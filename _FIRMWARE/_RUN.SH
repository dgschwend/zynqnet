##### EXECUTE BEFORE RUNNING:
axi_hp_config 0 32
loadbit zynqnet_200MHz.bit

#### EXECUTE ON CPU
./test CPU indata.bin

#### EXECUTE ON FPGA
./test FGPA indata.bin
