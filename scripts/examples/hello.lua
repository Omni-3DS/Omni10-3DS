-- Omni10 Lua Example – Hello World + System Info

ui.echo("Welcome to Omni10-3DS Lua!")

local info = sys.info()

local msg = string.format(
    "Model: %s\nRegion: %s\nOmni10 Version: %s",
    info.model or "Unknown",
    info.region or "Unknown",
    sys.version()
)

ui.echo(msg)

if ui.ask("Would you like to start the FTP server?") then
    net.ftp_start(5000, "omni", "omni10")
    ui.echo("FTP Server started on port 5000\nUser: omni\nPass: omni10")
    ui.echo("Press A to stop the server.")
    net.ftp_stop()
    ui.echo("FTP Server stopped.")
end

ui.echo("Script finished. Have a nice day!")
