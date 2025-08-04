# ChatTools Support Addon
## Build
```
cd win7
msbuild Win7Notify.sln /p:Platform=win32 /p:Configuration=Release
cd win10
cargo build --release
```