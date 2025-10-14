#!/usr/bin/env python3
import os
import sys
import yfinance as yf
from azure.storage.blob import BlobServiceClient
from azure.core.exceptions import ResourceNotFoundError

# Configuration
BLOB_CONTAINER   = "tickers-data"
LOCAL_DATA_DIR   = "data"

# Ensure local data folder exists
os.makedirs(LOCAL_DATA_DIR, exist_ok=True)

# Read connection string
conn_str = os.getenv("AZURE_STORAGE_CONNECTION_STRING")
if not conn_str:
    raise RuntimeError("AZURE_STORAGE_CONNECTION_STRING environment variable not set")

# Blob client
svc_client       = BlobServiceClient.from_connection_string(conn_str)
container_client = svc_client.get_container_client(BLOB_CONTAINER)

def scrape_and_upload(ticker: str):
    """Pull from Yahoo Finance, save CSV locally, then upload to blob."""
    csv_path = os.path.join(LOCAL_DATA_DIR, f"{ticker}.csv")
    print(f"⚙️  Scraping {ticker}…")
    df = yf.Ticker(ticker).history(period="5y")[["Close"]].reset_index()
    df.to_csv(csv_path, index=False)
    print(f"✅ Saved {csv_path}")

    blob = container_client.get_blob_client(f"{ticker}.csv")
    with open(csv_path, "rb") as f:
        blob.upload_blob(f, overwrite=True)
    print(f"⬆️  Uploaded {ticker}.csv")

def fetch(ticker: str):
    """Download CSV if present; else scrape_and_upload then retry."""
    local_path  = os.path.join(LOCAL_DATA_DIR, f"{ticker}.csv")
    blob_client = container_client.get_blob_client(f"{ticker}.csv")

    try:
        print(f"⬇️  Downloading {ticker}.csv…")
        downloader = blob_client.download_blob()
        with open(local_path, "wb") as f:
            f.write(downloader.readall())
        print(f"✅ Downloaded to {local_path}")

    except ResourceNotFoundError:
        print(f"⚠️  {ticker}.csv not found in blob; scraping now.")
        scrape_and_upload(ticker)
        fetch(ticker)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 fetchData.py TICKER1 [TICKER2 …]")
        sys.exit(1)
    for tk in sys.argv[1:]:
        fetch(tk.upper())
