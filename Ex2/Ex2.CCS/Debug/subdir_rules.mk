################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
adc.obj: ../adc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/amir/ti/ccsv6/tools/compiler/msp430_15.12.2.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/amir/ti/ccsv6/ccs_base/msp430/include" --include_path="/home/amir/ti/ccsv6/tools/compiler/msp430_15.12.2.LTS/include" --advice:power=all -g --define=__MSP430FR5739__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="adc.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/amir/ti/ccsv6/tools/compiler/msp430_15.12.2.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/amir/ti/ccsv6/ccs_base/msp430/include" --include_path="/home/amir/ti/ccsv6/tools/compiler/msp430_15.12.2.LTS/include" --advice:power=all -g --define=__MSP430FR5739__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="main.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

system.obj: ../system.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/amir/ti/ccsv6/tools/compiler/msp430_15.12.2.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/amir/ti/ccsv6/ccs_base/msp430/include" --include_path="/home/amir/ti/ccsv6/tools/compiler/msp430_15.12.2.LTS/include" --advice:power=all -g --define=__MSP430FR5739__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="system.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

uart.obj: ../uart.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"/home/amir/ti/ccsv6/tools/compiler/msp430_15.12.2.LTS/bin/cl430" -vmspx --use_hw_mpy=F5 --include_path="/home/amir/ti/ccsv6/ccs_base/msp430/include" --include_path="/home/amir/ti/ccsv6/tools/compiler/msp430_15.12.2.LTS/include" --advice:power=all -g --define=__MSP430FR5739__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="uart.d" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


