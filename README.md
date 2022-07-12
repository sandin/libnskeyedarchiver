# libnskeyedarchiver
A library for encoding and decoding apple NSKeyedArchiver

## Usage

Unarchive binary plist file:
```c++
FILE* f = fopen("setconfig.bplist", "rb");
char* buffer = nullptr; 
size_t buffer_size = 0;
read_content_from_file(f, &buffer, &buffer_size);

NSObject* obj = NSKeyedUnarchiver::UnarchiveTopLevelObjectWithData(
    buffer, buffer_size);
printf("%s\n", obj->ToString());
```
output:
```
[NSDictionary, length=3, items=[
    "rp" : 10,
    "tc" : [NSArray, length=1, items=[
                [NSDictionary, length=3, items=[
                    "uuid" : [NSString, data="2C46B61A-CDA9-4D59-B901-22E28B08C260"],
                    "tk" : 3,
                    "kdf2" : [NSSet, length=3, items=[630784000, 833617920, 830472456]]
                ]
            ]],
    "ur" : 5000
]]

```
