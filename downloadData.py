import yfinance as yf
import sys
import os

def download_and_save(ticker, start="2018-01-01", end="2023-01-01"):
    print(f"Downloading {ticker}...")
    data = yf.Ticker(ticker).history(start=start, end=end)
    if data.empty:
        print(f"⚠️ No data for {ticker}. Skipping.")
        return

    output_dir = "C:/Users/henry/MySideProject/quant-pairs/data"
    os.makedirs(output_dir, exist_ok=True)

    file_path = os.path.join(output_dir, f"{ticker.upper()}.csv")
    data[["Close"]].reset_index().to_csv(file_path, index=False)
    print(f"Saved {ticker} data to: {file_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python downloadData.py TICKER1 TICKER2")
        sys.exit(1)

    ticker1 = sys.argv[1].upper()
    ticker2 = sys.argv[2].upper()

    download_and_save(ticker1)
    download_and_save(ticker2)
