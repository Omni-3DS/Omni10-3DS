# Omni10 .slr → .o10

`.slr` = readable high-level script  
`.o10` = compact bytecode for the console (hard to read)

## Pipeline
```
script.slr  --[slr-compiler C#/C++]-->  script.o10  --[Omni10 FIRM]--> run
```

## .slr example
```
# hello.slr
print "Hello from SLR"
wait 500
if console == new3ds
  print "New 3DS"
else
  print "Old 3DS"
end
print "Done"
```

## Planned .o10 bytecode
- Header: `O10\0` + version + size
- Opcodes: PRINT, WAIT, CLEAR, INFO, POWEROFF, REBOOT, JMP, JZ, ...
- Strings in a pool (not plain text in the middle of the file)

## Compiler status
- Spec: this document
- Tooling: `tools/slr-compiler/` (next)
- Runtime: current FIRM still uses text `.o10` demos
