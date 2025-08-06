using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace QuantDashboard.Services
{
    public class BacktestExecute
    {
        public async Task<string> RunAsync(string ticker1, string ticker2)
        {
            var baseDir = AppContext.BaseDirectory;
            string fetcherPath = Path.Combine(baseDir, "fetchData.py");
            string backtesterExe = Path.Combine(baseDir, "pairs_backtester");
            string plotterPath = Path.Combine(baseDir, "plotter.py");

            try
            {
                // 1) Fetch or scrape/upload as needed
                var fetchInfo = new ProcessStartInfo(
                    "/usr/bin/python3",
                    $"\"{fetcherPath}\" {ticker1} {ticker2}")
                {
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    WorkingDirectory = baseDir
                };
                using var fetch = Process.Start(fetchInfo)
                    ?? throw new Exception("Could not start fetchData.py");
                var fetchErr = await fetch.StandardError.ReadToEndAsync();
                await fetch.WaitForExitAsync();
                if (!string.IsNullOrWhiteSpace(fetchErr))
                    return $"Fetch Error: {fetchErr.Trim()}";

                // 2) Run the C++ backtester
                var btInfo = new ProcessStartInfo(
                    backtesterExe,
                    $"{ticker1} {ticker2}")
                {
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    WorkingDirectory = baseDir
                };
                using var bt = Process.Start(btInfo)
                    ?? throw new Exception("Could not start pairs_backtester");
                var btOut = await bt.StandardOutput.ReadToEndAsync();
                var btErr = await bt.StandardError.ReadToEndAsync();
                await bt.WaitForExitAsync();
                if (!string.IsNullOrWhiteSpace(btErr))
                    return $"Backtester Error: {btErr.Trim()}";

                // 3) Extract the Total return line
                var lines = btOut
                    .Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
                var returnLine = lines
                    .FirstOrDefault(l => l.Trim().StartsWith("Total return", StringComparison.OrdinalIgnoreCase))
                    ?? lines.Last().Trim();

                // 4) Regenerate the plot
                var plotInfo = new ProcessStartInfo(
                    "/usr/bin/python3",
                    $"\"{plotterPath}\"")
                {
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    WorkingDirectory = baseDir
                };
                using var plot = Process.Start(plotInfo)
                    ?? throw new Exception("Could not start plotter.py");
                var plotErr = await plot.StandardError.ReadToEndAsync();
                await plot.WaitForExitAsync();
                if (!string.IsNullOrWhiteSpace(plotErr))
                    Console.WriteLine($"[Plotter Warning] {plotErr.Trim()}");

                // 5) Return the clean message
                return returnLine;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"[ERROR] {ex}");
                return $"Execution Error: {ex.Message}";
            }
        }
    }
}
