import os
import pandas as pd
import matplotlib.pyplot as plt

# Get the correct file path
script_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.dirname(script_dir)
trades_path = os.path.join(project_root, "docs", "trades.csv")

# Read the data
df = pd.read_csv(trades_path)

# Plot configuration
plt.figure(figsize=(12, 6))

# Cumulative return line
plt.plot(df["Date"], df["CumulativePnL"], label="Cumulative P&L", color="blue")

# Mark trades
buy_signals = df[df["Signal"] == 1]
sell_signals = df[df["Signal"] == -1]

plt.scatter(buy_signals["Date"], buy_signals["CumulativePnL"], color="green", label="Long Entry", marker="^")
plt.scatter(sell_signals["Date"], sell_signals["CumulativePnL"], color="red", label="Short Entry", marker="v")

plt.title("Strategy Cumulative P&L and Trades")
plt.xlabel("Date")
plt.ylabel("Cumulative Return")
plt.xticks(rotation=45)
plt.legend()
plt.tight_layout()
plt.savefig("strategy_performance.png")  # Save instead of show
plt.show()