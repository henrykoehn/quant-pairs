import os
import sys
import yfinance as yf
import pandas as pd
from azure.storage.blob import BlobServiceClient

# 1) Read the connection string from environment
conn_str = os.getenv("AZURE_STORAGE_CONNECTION_STRING")
if not conn_str:
    raise RuntimeError("AZURE_STORAGE_CONNECTION_STRING not set")

# 2) Create the BlobServiceClient and container client
blob_service = BlobServiceClient.from_connection_string(conn_str)
container = blob_service.get_container_client("tickers-data")

def download_and_upload(ticker: str,
                        start: str = "2018-01-01",
                        end:   str = "2023-01-01"):
    """
    Downloads historical Close prices for ticker from Yahoo Finance,
    writes a temporary CSV, then uploads it to Azure Blob Storage.
    """
    print(f"Downloading {ticker}...")
    df = yf.Ticker(ticker).history(start=start, end=end)

    if df.empty:
        print(f"No data for {ticker}, skipping.")
        return

    # Keep only Date + Close
    df = df[["Close"]].reset_index()
    local_csv = f"{ticker}.csv"
    df.to_csv(local_csv, index=False)

    # Upload (overwrite) to blob container
    print(f"Uploading {ticker}.csv to Azure Blob Storage...")
    with open(local_csv, "rb") as data:
        container.upload_blob(name=f"{ticker}.csv",
                              data=data,
                              overwrite=True)

    print(f"{ticker}.csv uploaded successfully.")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python downloadData.py TICKER1 TICKER2")
        sys.exit(1)

    ticker1 = sys.argv[1].upper()
    ticker2 = sys.argv[2].upper()

    download_and_upload(ticker1)
    download_and_upload(ticker2)
