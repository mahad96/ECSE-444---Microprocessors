	AREA test, CODE, READONLY
	
	export asm_sd                  
asm_sd

	MOV R4, #0
	VMOV.f32 S0, R4
	VMOV.f32 S1, R4
	VMOV.f32 S2, R4
	VMOV.f32 S3, R4
	VMOV.f32 S4, R4
	
	
	PUSH{R0-R3, LR}
	BL mean
	POP{R0-R3, LR}
	MOV R4, R1
	
var_loop
	SUBS R4, R4, #1				
	BLT done
	VLDR.f32 S2, [R0], #4		
	VSUB.f32 S3, S2, S0			
	VMUL.f32 S3, S3, S3			
	VADD.f32 S4, S3, S4			
	B var_loop

mean
	MOV R4, R1
mean_loop
	SUBS R4, R4, #1				
	BLT mean_done				
	VLDR.f32 S0, [R0], #4		
	VADD.f32 S1, S0, S1			
	B mean_loop
	
mean_done
	MOV R4, R1					
	VMOV.f32 S5, R4
	VCVT.f32.s32 S5, S5
	VDIV.f32 S0, S1, S5			
	BX LR
	
done
	SUB R1, R1, #1
	VMOV.f32 S5, R1
	VCVT.f32.s32 S5, S5
	VDIV.f32 S4, S4, S5		
	VSQRT.f32 S4, S4
	VSTR.f32 S4, [R2]
	BX LR
	
	END