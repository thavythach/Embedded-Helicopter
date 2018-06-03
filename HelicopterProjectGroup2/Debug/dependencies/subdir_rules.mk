################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
dependencies/ustdlib.obj: C:/Users/Gabriel/Desktop/UC/Embedded-Systems-1/HelicopterProject/dependencies/ustdlib.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Gabriel/workspace_v7/HelicopterProjectGroup2" --include_path="C:/ti/TivaWare_C_Series-2.1.4.178" --include_path="C:/Users/Gabriel/Desktop/UC/Embedded-Systems-1/source_code" --include_path="C:/Users/Gabriel/Desktop/UC/Embedded-Systems-1/source_code/lab_a03_group_2/Milestones/Milestone" --include_path="C:/Users/Gabriel/git/lab_a03_group_2___latest/Milestones/Milestone" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="dependencies/ustdlib.d_raw" --obj_directory="dependencies" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


