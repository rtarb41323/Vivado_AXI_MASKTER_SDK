# Vivado_AXI_MASTER_SDK
This is the SDK side of the Vivado_HLS_AXI_MASTER repository.  Inclucded are:

1. Block diagram picture of the custom accelerator added to a ZYNQ processing system

2. The accompanying memory map that shows the addresses of the blocks (both the 
HP0 port for the AXI master, and the AXI Lite port)

3. The "helloworld.c" file used in the SDK.  After exporting the generated bitstream file, and
launching Vivado SDK, go to "File" -> "New" -> "Application Project", name your project, and
select the "Helloworld.c" project template.  THEN, replace the contents of the "Helloworld.c" 
file with the "Helloworld.c" file in THIS repository.

