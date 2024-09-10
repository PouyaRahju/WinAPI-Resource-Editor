# WinAPI Resource Editor
 This project provides a set of functions for adding, reading, and verifying resources, making it easier to manipulate and manage custom data embedded in Windows executable files.

# Key Features:
- Add Text Resources: Add custom text resources to an executable file. This function allows you to insert textual data, which can be useful for embedding configuration settings or other data.

- Read Embedded Text Resources: Retrieve and read text resources from the executable. This can be useful for extracting configuration data or other embedded information.

- Add Binary Resources: Add binary resources from external files to an executable. This supports embedding various types of binary data, such as images or other files.

- Extract Resources: Extract embedded resources from the executable to a file. This function supports extracting both text and binary resources, facilitating easy access to embedded data.

- Check Resource Existence: Verify if a specific resource exists within an executable. This function helps in validating the presence of resources before attempting to read or manipulate them.

- Self-Resource Handling: Special functions to manage and access resources embedded within the same executable file running the code.


# Usage Examples

- Adding a Text Resource
```
AddTextResource("myapp.exe", "Sample Text", "RESOURCE_NAME");
```

- Reading a Text Resource
```
char output[256];
memset(output,'\0',sizeof(output));
ReadSelfResourceText("RESOURCE_NAME", output, sizeof(output));
```

- Adding a Binary Resource
```
AddResource("myapp.exe", "image.png", "RESOURCE_NAME");
```

- Extracting a Resource
```
ReadSelfResource("output.png", "RESOURCE_NAME");
```

- Checking Resource Existence
```
ResourceExists("myapp.exe", "RESOURCE_NAME", RT_RCDATA);
```
or
```
ResourceSelfExists("RESOURCE_NAME", RT_RCDATA);
```


License:

This project is licensed under the MIT License. See the LICENSE file for more details.
