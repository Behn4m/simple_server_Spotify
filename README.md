| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Simple Server Spotify
this code aims to provide and test an interface for Spotify API to authenticate access and control music playback

## Project structure
*to be completed*


## How componenets are initiateed in main
Initialization of SpotifyInterfaceHandler involves setting up its fields, including connecting it to the HTTPS request infrastructure (HttpsBufQueue, semaphores, etc.) and providing callback functions. Proper initialization ensures that the Spotify interface can function correctly.
`HttpsRequestsHandler` and `SpotifyInterfaceHandler` are structures that hold configurations and parameters related to the HTTPS requests and Spotify interface components, respectively. They are used to organize and pass information between different parts of your program. 
* `HttpsRequestsHandler` is the structure or object that holds information necessary for handling HTTPS requests. It likely includes parameters like ResponseBufQueue (a queue for response buffers) and ResponseReadySemaphore (a semaphore indicating that an HTTPS response is ready).

* `SpotifyInterfaceHandler` is the structure or object that holds configurations for interfacing with Spotify. It includes parameters like HttpsBufQueue (a queue for HTTPS requests related to Spotify), semaphores, and callback functions.


## How to enable Debug for ESP32-S3

* Make sure you followed steps in: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/jtag-debugging/
* Make sure you parameters are set as follwoing in `setting.json` file of your active project:

  ```
  "idf.openOcdConfigs": [
      "esp32s3-builtin.cfg",
      "target/esp32s3.cfg"
  ```
* use `openocd -f board/esp32s3-builtin.cfg` to run OPENOCD
* For more detailed debug report, enable heap positioning and tracing in menuconfig. The result should be displayed as follow in `sdkconfig` file
  ```
  CONFIG_APPTRACE_DEST_JTAG=y
  ...
  CONFIG_APPTRACE_DEST_TRAX=y
  CONFIG_APPTRACE_MEMBUFS_APPTRACE_PROTO_ENABLE=y
  CONFIG_APPTRACE_ENABLE=y
  ...
  CONFIG_APPTRACE_ONPANIC_HOST_FLUSH_TMO=-1
  CONFIG_APPTRACE_POSTMORTEM_FLUSH_THRESH=0
  CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX=0
  ```

* use debugger interface in VSCode - you may need to update some more configuration and setting files
