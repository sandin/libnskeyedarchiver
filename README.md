# libnskeyedarchiver
A library for encoding and decoding Apple's NSKeyedArchiver.

## Usage

### Library

Unarchive a binary plist file:
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

