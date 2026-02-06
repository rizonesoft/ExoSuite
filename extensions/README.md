# Extensions

This directory contains ExoSuite extension modules that integrate into the main application.

## Structure

Each extension has its own subdirectory:

```
extensions/
├── SystemProperties/     # System Properties utility
├── NetworkSettings/      # Network configuration
└── ...
```

## Creating an Extension

1. Create a subdirectory: `extensions/YourExtension/`
2. Add source files and a local `CMakeLists.txt`
3. Use the `add_exosuite_extension()` macro in the parent CMakeLists.txt
4. Extensions output to `build/bin/system/` as DLLs

## Extension Template

See `docs/dev/extension-architecture.md` for the extension API and development guide.
