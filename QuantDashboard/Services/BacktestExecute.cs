using System;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;

namespace QuantDashboard.Services
{
    public class BacktestExecute
    {
        public async Task<string> RunAsync(string ticker1, string ticker2)
        {
            // Paths to your scripts/executable
            string scraperPath = @"C:\Users\henry\MySideProjects\quant-pairs\downloadData.py";
            string backtesterPath = @"C:\Users\henry\MySideProjects\quant-pairs\cpp\cmake-build-debug\pairs_backtester.exe";
            string plotterPath = @"C:\Users\henry\MySideProjects\quant-pairs\plotter\plotter.py";

            try
            {
                // 1) Run the Python scraper
                var pyInfo = new ProcessStartInfo("python", $"\"{scraperPath}\" {ticker1} {ticker2}")
                {
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                };
                using (var py = Process.Start(pyInfo))
                {
                    if (py == null) throw new Exception("Failed to start scraper");
                    string err = await py.StandardError.ReadToEndAsync();
                    await py.WaitForExitAsync();
                    if (!string.IsNullOrWhiteSpace(err))
                        return $"Python Error: {err}";
                }

                // 2) Run the C++ backtester and capture its output
                string backtesterOutput;
                var btInfo = new ProcessStartInfo(backtesterPath, $"{ticker1} {ticker2}")
                {
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                };
                using (var bt = Process.Start(btInfo))
                {
                    if (bt == null) throw new Exception("Failed to start backtester");
                    backtesterOutput = await bt.StandardOutput.ReadToEndAsync();
                    string btErr = await bt.StandardError.ReadToEndAsync();
                    await bt.WaitForExitAsync();
                    if (!string.IsNullOrWhiteSpace(btErr))
                        return $"Backtester Error: {btErr}";
                }

                // 3) Parse out just the Total return line
                var lines = backtesterOutput
                    .Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
                string returnLine = lines
                    .FirstOrDefault(l => l.Trim().StartsWith("Total return", StringComparison.OrdinalIgnoreCase))
                    ?? lines.Last();

                // 4) Regenerate the plot image
                var plotInfo = new ProcessStartInfo("python", $"\"{plotterPath}\"")
                {
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                };
                using (var plot = Process.Start(plotInfo))
                {
                    if (plot == null) throw new Exception("Failed to start plotter");
                    string plotErr = await plot.StandardError.ReadToEndAsync();
                    await plot.WaitForExitAsync();
                    if (!string.IsNullOrWhiteSpace(plotErr))
                        Console.WriteLine($"Plotter warning: {plotErr}");
                }

                // 5) Return only the clean Total return
                return returnLine;
            }
            catch (Exception ex)
            {
                // Log to console so you can see it in your 'dotnet run' window
                Console.WriteLine($"[ERROR] {ex}");
                return $"Execution Error: {ex.Message}";
            }
        }
    }
}
