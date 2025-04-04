#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "rv32.h"
#include "uart.h"
#include "memmappeddevice.h"

#define CSR_MSTATUS			0x300
#define CSR_MISA			0x301
#define CSR_MIE		    	0x304
#define CSR_MTVEC			0x305
#define CSR_MSCRATCH		0x340
#define CSR_MEPC			0x341
#define CSR_MCAUSE			0x342
#define CSR_MTVAL			0x343
#define CSR_MIP				0x344
#define CSR_TIMECMPLO		0x800
#define CSR_TIMECMPHI		0x801
#define CSR_CYCLELO			0xC00
#define CSR_TIMELO			0xC01
#define CSR_RETILO			0xC02
#define CSR_CYCLEHI			0xC80
#define CSR_TIMEHI			0xC81
#define CSR_RETIHI			0xC82
#define CSR_MARCHID			0xF12
#define CSR_MIMPID			0xF13
#define CSR_MHARTID			0xF14
#define CSR_REGISTERSHADOW	0x8A0

// Custom CSRs
#define CSR_CPURESET		0xFEE
#define CSR_WATERMARK		0xFF0
#define CSR_PROGRAMCOUNTER	0xFFC
#define CSR_HWSTATE			0xFFF

class CCSRMem : public MemMappedDevice
{
public:
	explicit CCSRMem(uint32_t hartid) : m_hartid(hartid) { m_csrmem = (uint32_t*)malloc(4096 * sizeof(uint32_t)); }
	~CCSRMem() { free(m_csrmem); }

	void Reset() override final;
	void Read(uint32_t address, uint32_t& data) override final;
	void Write(uint32_t address, uint32_t word, uint32_t wstrobe) override final;
	void Tick(CBus* bus);
	uint32_t* GetCSRMem() { return m_csrmem; }

	void UpdateTime(uint64_t wallclock, uint32_t executeCount);
	void SetRetiredInstructions(uint64_t retired) { m_retired = retired; }
	void SetPC(uint32_t pc) { m_pc = pc; }
	void RequestReset() { m_cpuresetreq = 1; }

	uint64_t m_retired{ 0 };
	uint64_t m_cycle{ 0 };
	uint64_t m_wallclocktime{ 0 };
	uint32_t m_hartid{ 0 };
	uint32_t m_pc{ 0 };

	uint64_t m_timecmpshadow{ 0 };
	uint32_t m_mepcshadow{ 0 };
	uint32_t m_mieshadow{ 0 };
	uint32_t m_mtvecshadow{ 0 };

	uint32_t m_sie{0};
	uint32_t m_irq{0};
	bool m_pendingCPUReset{false};

private:
	uint32_t* m_csrmem{ nullptr };
	uint32_t m_cpuresetreq{ 0 };
	uint32_t m_mstatusieshadow{ 0 };
	uint32_t m_irqstate{ 0 };
};
