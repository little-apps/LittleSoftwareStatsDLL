# LittleSoftwareStatsDLL
This is a Dynamic Linking Library (DLL) for integrating the API for [Little Software Stats](https://github.com/little-apps/little-software-stats) with various programs. It is a Little Apps project and is coded in C++. 

### Installation ###
Please either look at the file INSTALL.txt (included with this project) or follow [these instructions](http://little-software-stats.com/docs/libraries/cplusplus-windows/) on the Little Software website for steps on integrating this library.

### License ###
This library is licensed under the [GNU Lesser General Public License v3](http://www.gnu.org/copyleft/lesser.html).

### Show Your Support ###
Little Apps relies on people like you to keep our software running. If you would like to show your support for Little Software Stats, then you can [make a donation](https://www.little-apps.com/?donate) using PayPal, Payza or Bitcoins. Please note that any amount helps (even just $1). 

### Notes ###
#### Operating System Detection ####
In order for the LittleSoftwareStatsDLL to properly detect operating systems, please ensure that your program has a manifest and contains the following compatibility entries. Without these, the detected operating system may be incorrect. See [this MSDN article](https://msdn.microsoft.com/en-us/library/windows/desktop/dn481241%28v=vs.85%29.aspx) for more information.

    <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1" xmlns:asmv3="urn:schemas-microsoft-com:asm.v3">
        <compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1"> 
            <application>
                <!-- Windows 10 -->
                <supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>
                <!-- Windows 8.1 -->
                <supportedOS Id="{1f676c76-80e1-4239-95bb-83d0f6d0da78}"/>
                <!-- Windows Vista -->
                <supportedOS Id="{e2011457-1546-43c5-a5fe-008deee3d3f0}"/> 
                <!-- Windows 7 -->
                <supportedOS Id="{35138b9a-5d96-4fbd-8e2d-a2440225f93a}"/>
                <!-- Windows 8 -->
                <supportedOS Id="{4a2f28e3-53b9-4441-ba9c-d69d4a4a6e38}"/>
            </application> 
        </compatibility>
    </assembly>

#### Other ####
 - This library should not require administrator privileges in order to work properly. However, the included DLL library (available for download from [getlittleapps.com](https://getlittleapps.com/little-software-stats/libraries/)) has already been code signed just in case.
 - If you need to make changes to the interaction with the API (such as how the response is handled), then you need to modify the code for the DLL.
 - Due to the limitations of C++, you (usually) cannot use the stack trace when tracking exceptions with Little Software Stats.
