/*
* Pouya Rahju
* https://github.com/PouyaRahju/WinAPI-Resource-Editor
* Add, read and check the existence of resources with WinAPI - C Programming
*/
#ifndef RESOURCE_CONTROL_H
#define RESOURCE_CONTROL_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RESDEBUG_PRINT 0
#define SUCCESS 0
#define ERRORRET 1


// Function to print error messages if RESDEBUG_PRINT is set to 1
void PrintDebug(LPCSTR msg)
{
    if (!RESDEBUG_PRINT)
        return; // If debugging is turned off, do nothing

    DWORD error = GetLastError(); // Retrieve the last error code
    LPVOID lpMsgBuf;
    
    // Convert error code to a human-readable message
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf,
        0, NULL);
    
    // Print the error message
    printf("%s: %s\n", msg, (LPSTR)lpMsgBuf);
    LocalFree(lpMsgBuf); // Free the memory allocated for the message
}


// Generate Random String
void generateRandomString(char *str, size_t length) {
    length--;
    srand(time(0));
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charsetSize = sizeof(charset) - 1; // -1 برای حذف null terminator

    for (size_t i = 0; i < length; i++) {
        int randomIndex = rand() % charsetSize;
        str[i] = charset[randomIndex];
    }
    str[length] = '\0'; // افزودن null terminator به انتهای رشته
}


// Function to add a text resource to an executable file
int AddTextResource(const char *targetFile, const char *text, const char *resourceName) {
    // Begin updating the resource of the target file
    HANDLE hUpdate = BeginUpdateResource(targetFile, FALSE);
    if (hUpdate == NULL) {
        PrintDebug("Failed to open file for updating resources: ");
        return(ERRORRET);
    }

    // Update the resource with the given text
    if (!UpdateResource(hUpdate, RT_RCDATA, resourceName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (void *)text, strlen(text) + 1)) {
        PrintDebug("Failed to update resource: ");
        EndUpdateResource(hUpdate, TRUE); // Revert changes on failure
        return(ERRORRET);
    }

    // Finalize the resource update
    if (!EndUpdateResource(hUpdate, FALSE)) {
        PrintDebug("Failed to finalize resource update: ");
        return(ERRORRET);
    } else {
        PrintDebug("Text resource added successfully.\n");
        return(SUCCESS);
    }
}

// Function to read a text resource from the executable file itself
int ReadSelfResourceText(const char *resourceName, char *output, size_t output_Count) {
    HMODULE hModule = GetModuleHandle(NULL); // Get handle to the current module
    if (hModule == NULL) {
        PrintDebug("Failed to get module handle: ");
        return(ERRORRET);
    }

    // Find the specified resource in the module
    HRSRC hResource = FindResource(hModule, resourceName, RT_RCDATA);
    if (hResource == NULL) {
        PrintDebug("Failed to find resource: ");
        return(ERRORRET);
    }

    // Load the resource
    HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
    if (hLoadedResource == NULL) {
        PrintDebug("Failed to load resource: ");
        return(ERRORRET);
    }

    // Lock the resource and get a pointer to its data
    char *pResourceData = (char *)LockResource(hLoadedResource);
    DWORD resourceSize = SizeofResource(hModule, hResource);

    if (pResourceData != NULL && resourceSize > 0) {
        // Copy the resource data to the output buffer
        strncpy(output, pResourceData , output_Count );
        return(SUCCESS);
    } else {
        PrintDebug("Failed to lock or size resource\n");
        return(ERRORRET);
    }
}

// Function to add a binary resource from a file to an executable file
int AddResource(const char *targetFile, const char *resourceFile, const char *resourceName) {
    // Begin updating the resource of the target file
    HANDLE hUpdate = BeginUpdateResource(targetFile, FALSE);
    if (hUpdate == NULL) {
        PrintDebug("Failed to open file for updating resources: ");
        return(ERRORRET);
    }

    // Open the resource file for reading
    HANDLE hResource = CreateFile(resourceFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hResource == INVALID_HANDLE_VALUE) {
        PrintDebug("Failed to open resource file: ");
        EndUpdateResource(hUpdate, TRUE); // Revert changes on failure
        return(ERRORRET);
    }

    DWORD fileSize = GetFileSize(hResource, NULL);
    char *buffer = (char *)malloc(fileSize);
    if (buffer == NULL) {
        PrintDebug("Memory allocation failed\n");
        CloseHandle(hResource);
        EndUpdateResource(hUpdate, TRUE); // Revert changes on failure
        return(ERRORRET);
    }

    // Read the resource file into the buffer
    ReadFile(hResource, buffer, fileSize, &fileSize, NULL);
    CloseHandle(hResource);

    // Update the resource of the target file with the read data
    if (!UpdateResource(hUpdate, RT_RCDATA , resourceName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), buffer, fileSize)) {
        PrintDebug("Failed to update resource: ");
        free(buffer);
        EndUpdateResource(hUpdate, TRUE); // Revert changes on failure
        return(ERRORRET);
    }

    free(buffer);
    // Finalize the resource update
    if (!EndUpdateResource(hUpdate, FALSE)) {
        PrintDebug("Failed to finalize resource update: ");
        return(ERRORRET);
    } else {
        PrintDebug("Resource added successfully.\n");
        return(SUCCESS);
    }
}

// Function to read a binary resource from the executable file itself
int ReadSelfResource(char *output_path,const char *resourceName) {
    HMODULE hModule = GetModuleHandle(NULL); // Get handle to the current module
    if (hModule == NULL) {
        PrintDebug("Failed to get module handle: ");
        return(ERRORRET);
    }

    // Find the specified resource in the module
    HRSRC hResource = FindResource(hModule, resourceName, RT_RCDATA);
    if (hResource == NULL) {
        PrintDebug("Failed to find resource: ");
        return(ERRORRET);
    }

    // Load the resource
    HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
    if (hLoadedResource == NULL) {
        PrintDebug("Failed to load resource: ");
        return(ERRORRET);
    }

    // Lock the resource and get a pointer to its data
    void *pResourceData = LockResource(hLoadedResource);
    DWORD resourceSize = SizeofResource(hModule, hResource);

    // Open a file to write the resource data
    FILE *file = fopen(output_path, "wb");
    if (file == NULL) {
        PrintDebug("Failed to open file for writing\n");
        return(ERRORRET);
    }

    // Write the resource data to the file
    fwrite(pResourceData, 1, resourceSize, file);
    fclose(file);
    PrintDebug("Resource extracted successfully.\n");
    return(SUCCESS);
}

// Reads a resource from the current executable and adds it to the specified target executable file.
int ReadSelfThenAdd(const char *resourceName, const char *targetFile) {
    // Get the module handle for the current executable
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        PrintDebug("Failed to get module handle: ");
        return(ERRORRET);
    }

    // Find the resource in the current executable
    HRSRC hResource = FindResource(hModule, resourceName, RT_RCDATA);
    if (hResource == NULL) {
        PrintDebug("Failed to find resource: ");
        return(ERRORRET);
    }

    // Load the resource
    HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
    if (hLoadedResource == NULL) {
        PrintDebug("Failed to load resource: ");
        return(ERRORRET);
    }

    // Lock the resource to get a pointer to its data
    void *pResourceData = LockResource(hLoadedResource);
    DWORD resourceSize = SizeofResource(hModule, hResource);

    // Open the target file for updating resources
    HANDLE hUpdate = BeginUpdateResource(targetFile, FALSE);
    if (hUpdate == NULL) {
        PrintDebug("Failed to open file for updating resources: ");
        return(ERRORRET);
    }

    // Update the target file with the resource data
    if (!UpdateResource(hUpdate, RT_RCDATA, resourceName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), pResourceData, resourceSize)) {
        PrintDebug("Failed to update resource: ");
        EndUpdateResource(hUpdate, TRUE);
        return(ERRORRET);
    }

    // Finalize the resource update
    if (!EndUpdateResource(hUpdate, FALSE)) {
        PrintDebug("Failed to finalize resource update: ");
        return(ERRORRET);
    } else {
        PrintDebug("Resource data read from self and added to target file successfully.\n");
        return(SUCCESS);
    }
}


// Function to check if a resource exists in a specified file
int ResourceExists(const char *fileName, const char *resourceName, const char *resourceType) {
    // Open the file for reading
    HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        PrintDebug("Failed to open file: ");
        return(ERRORRET);
    }

    // Create a file mapping object
    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMapping == NULL) {
        PrintDebug("Failed to create file mapping: ");
        CloseHandle(hFile);
        return(ERRORRET);
    }

    // Map a view of the file into the address space
    LPVOID pMapView = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
    if (pMapView == NULL) {
        PrintDebug("Failed to map view of file: ");
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return(ERRORRET);
    }

    // Load the file as a library to search for resources
    HMODULE hModule = LoadLibraryEx(fileName, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (hModule == NULL) {
        PrintDebug("Failed to load library: ");
        UnmapViewOfFile(pMapView);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return(ERRORRET);
    }

    // Find the specified resource in the loaded library
    HRSRC hResource = FindResource(hModule, resourceName, resourceType);
    if (hResource != NULL) {
        PrintDebug("Resource found.\n");
        UnmapViewOfFile(pMapView);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        FreeLibrary(hModule);
        return(SUCCESS);
    } else {
        PrintDebug("Resource not found.\n");
        UnmapViewOfFile(pMapView);
        CloseHandle(hMapping);
        CloseHandle(hFile);
        FreeLibrary(hModule);
        return(ERRORRET);
    }
}

// Function to check if a resource exists in the current executable file
int ResourceSelfExists(const char *resourceName, const char *resourceType) {
    // Get handle to the current module
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == NULL) {
        PrintDebug("Failed to get module handle: ");
        return(ERRORRET);
    }

    // Find the specified resource in the current module
    HRSRC hResource = FindResource(hModule, resourceName, resourceType);
    if (hResource == NULL) {
        // Resource not found
        return(ERRORRET);
    }

    // Resource found
    return(SUCCESS);
}

#endif
