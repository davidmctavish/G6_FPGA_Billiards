@echo off
rem  Vivado(TM)
rem  compile.bat: a Vivado-generated XSim simulation Script
rem  Copyright 1986-2014 Xilinx, Inc. All Rights Reserved.

set PATH=%XILINX%\lib\%PLATFORM%;%XILINX%\bin\%PLATFORM%;C:/Xilinx/SDK/2014.1/bin;C:/Xilinx/Vivado/2014.1/ids_lite/ISE/bin/nt64;C:/Xilinx/Vivado/2014.1/ids_lite/ISE/lib/nt64;C:/Xilinx/Vivado/2014.1/bin;%PATH%
set XILINX_PLANAHEAD=C:/Xilinx/Vivado/2014.1

xelab -m64 --debug typical --relax -L xil_defaultlib -L microblaze_v9_3 -L lmb_v10_v3_0 -L lmb_bram_if_cntlr_v4_0 -L blk_mem_gen_v8_2 -L fifo_generator_v12_0 -L proc_common_v4_0 -L axi_lite_ipif_v2_0 -L mdm_v3_1 -L proc_sys_reset_v5_0 -L xbip_utils_v3_0 -L axi_utils_v2_0 -L xbip_pipe_v3_0 -L xbip_dsp48_wrapper_v3_0 -L xbip_dsp48_addsub_v3_0 -L xbip_bram18k_v3_0 -L mult_gen_v12_0 -L floating_point_v7_0 -L xbip_dsp48_mult_v3_0 -L xbip_dsp48_multadd_v3_0 -L div_gen_v5_1 -L unisims_ver -L unimacro_ver -L secureip --snapshot tb_behav --prj C:/billiards/PositionLocatorTest/PositionLocatorTest_2014_1/PositionLocatorTest_2014_1.sim/sim_1/behav/tb.prj   xil_defaultlib.tb   xil_defaultlib.glbl
if errorlevel 1 (
   cmd /c exit /b %errorlevel%
)
