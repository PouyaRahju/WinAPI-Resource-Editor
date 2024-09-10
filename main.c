/*
* Pouya Rahju
* https://github.com/PouyaRahju/WinAPI-Resource-Editor
* Add, read and check the existence of resources with WinAPI - C Programming
*/
#include <windows.h>
#include <stdio.h>
#include "ResourceControl.h"

int main() {


    
    int ret= AddResource("target.exe", "Screenshot.png", "RESOURCE_KEY_NAME");
    if (ret == SUCCESS)
    {
        printf("Successfuly Resource Added.");
    }


    // int ret = ReadSelfResource("test.png", "RESOURCE_KEY_NAME");
    // if (ret == SUCCESS)
    // {
    //     printf("SUCESS");
    // }

    return 0;
}
