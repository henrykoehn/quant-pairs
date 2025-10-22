############################################
# Stage A: Build the C++ backtester on Linux
############################################
FROM debian:bookworm-slim AS build-cpp

# Install build essentials
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      build-essential cmake \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src/cpp
COPY cpp/ .
RUN mkdir build \
 && cd build \
 && cmake -DCMAKE_BUILD_TYPE=Release .. \
 && cmake --build . --target pairs_backtester

############################################
# Stage B: Build & publish Blazor app
############################################
FROM mcr.microsoft.com/dotnet/sdk:7.0 AS build-blazor

WORKDIR /src
COPY QuantDashboard/QuantDashboard.csproj ./QuantDashboard/
RUN dotnet restore QuantDashboard/QuantDashboard.csproj

COPY QuantDashboard/ ./QuantDashboard/
WORKDIR /src/QuantDashboard
RUN dotnet publish -c Release -o /app/publish

############################################
# Stage C: Final runtime image (Linux)
############################################
FROM mcr.microsoft.com/dotnet/aspnet:7.0 AS runtime

WORKDIR /app

# 1) Install Python3 & pip + shim 'python' → 'python3'
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      python3 python3-pip python-is-python3 ca-certificates \
 && rm -rf /var/lib/apt/lists/*

# 2) Create shared folders for C++ and Python
#    - /app/data      → for CSVs downloaded by fetchData.py and read by pairs_backtester
#    - /app/wwwroot   → for trades.csv output served by Blazor
RUN mkdir -p /app/data /app/wwwroot

# 3) Copy Blazor site
COPY --from=build-blazor /app/publish ./

# 4) Copy Linux-built backtester
COPY --from=build-cpp /src/cpp/build/pairs_backtester ./pairs_backtester
RUN chmod +x ./pairs_backtester

# 5) Copy Python scripts & requirements
COPY requirements.txt .
COPY fetchData.py       .
COPY plotter/plotter.py ./plotter.py

# 6) Install Python deps
RUN python3 -m pip install --no-cache-dir -r requirements.txt

# 7) Expose & run
EXPOSE 80
ENTRYPOINT ["dotnet", "QuantDashboard.dll"]

