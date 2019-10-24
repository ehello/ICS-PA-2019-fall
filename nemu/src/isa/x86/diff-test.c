#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if(ref_r->eax !=cpu.eax || ref_r->ecx != cpu.ecx
      ||ref_r->edx != cpu.edx||ref_r->ebx != cpu.ebx
      ||ref_r->esp != cpu.esp||ref_r->ebp != cpu.ebp
      ||ref_r->esi != cpu.esi|| ref_r->edi != cpu.edi
      ||pc != cpu.pc){
        printf("ref's eax is 0x%x\n",ref_r->eax);
        printf("ref's ecx is 0x%x\n",ref_r->ecx);
        printf("ref's edx is 0x%x\n",ref_r->edx);
        printf("ref's ebx is 0x%x\n",ref_r->ebx);
        printf("ref's esp is 0x%x\n",ref_r->esp);
        printf("ref's ebp is 0x%x\n",ref_r->ebp);
        printf("ref's esi is 0x%x\n",ref_r->esi);
        printf("ref's edi is 0x%x\n",ref_r->edi);
        printf("pc is 0x%x, cpu.pc is 0x%x\n",pc,cpu.pc);
        return false;
      }
    
  return true;
}

void isa_difftest_attach(void) {
}
