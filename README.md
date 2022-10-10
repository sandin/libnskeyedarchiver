# libnskeyedarchiver
A library for encoding and decoding Apple's NSKeyedArchiver.

## Building

### Requirements

```
$ brew install cmake libplist
$ brew install libimobiledevice
#   ...
#   Pouring libimobiledevice--1.3.0.monterey.bottle.tar.gz
#   /usr/local/Cellar/libimobiledevice/1.3.0
```

To build the `NSKeyedArchiver` library, clone the repository and do `cmake` smoketest:

```
$ git clone git@github.com:sandin/libnskeyedarchiver.git
Cloning into 'libnskeyedarchiver'...

$ cd libnskeyedarchiver
$ IMB_PKG_CONFIG_PATH=/usr/local/Cellar/libimobiledevice/1.3.0 cmake .

#    -- Checking for module 'libplist-2.0'
#    --   Found libplist-2.0, version 2.2.0
#    -- Configuring done
#    -- Generating done
#    -- Build files have been written to: ~/dev/libnskeyedarchiver
```

This will build a target bundled with static library (`libnskeyedarchiver.a`):

```
$ IMB_PKG_CONFIG_PATH=/usr/local/Cellar/libimobiledevice/1.3.0 cmake --build . --target nskeyedarchiver
[ 16%] Building CXX object CMakeFiles/nskeyedarchiver.dir/src/base64.cpp.o
[ 33%] Building CXX object CMakeFiles/nskeyedarchiver.dir/src/nstypes.cpp.o
...
[100%] Linking CXX static library libnskeyedarchiver.a
[100%] Built target nskeyedarchiver
```


This will build a `nskeyedarchiver_tools` Mach-O binary:

```
$ IMB_PKG_CONFIG_PATH=/usr/local/Cellar/libimobiledevice/1.3.0 cmake --build . --target nskeyedarchiver_tools
...
[ 87%] Building CXX object CMakeFiles/nskeyedarchiver_tools.dir/tools/main.cpp.o
[100%] Linking CXX executable nskeyedarchiver_tools
[100%] Built target nskeyedarchiver_tools
```

The final binaries will be in deployed in current working directory. Finally, you can move built binaries to newly created `/bin` directory (ie. `mv libnskeyedarchiver.a nskeyedarchiver_tools ./bin`).

## Usage

### Library

**Decode a binary plist file:**
```c++
KAValue obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(
    buffer, (uint32_t)buffer_size);
printf("%s\n", obj->ToJson().c_str());
```
output:
```json
{
    "rp": 10,
    "tc": [
        {
            "kdf2": [
                630784000,
                833617920,
                830472456
            ],
            "tk": 3,
            "uuid": "2C46B61A-CDA9-4D59-B901-22E28B08C260"
        }
    ],
    "ur": 500
}
```

**Encode a object to binary plist format:**
```c++
const KAValue& object = KAValue(true);
char* buffer = nullptr;
size_t buffer_size = 0;
NSKeyedArchiver::ArchivedData(object, &buffer, &buffer_size,
                            NSKeyedArchiver::OutputFormat::Binary);
```


### Tools

Use command line tools to parse bplist files into json format:
```
$ nskeyedarchiver_tools bplist2json test.bplist
```
output:
```json
{
    "hwCPU64BitCapable": 1,
    "hwCPUsubtype": 2,
    "hwCPUtype": 16777228,
    "numberOfCpus": 6,
    "numberOfPhysicalCpus": 6
}
```

