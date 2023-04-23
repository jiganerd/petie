/*
a Herwin production '98
version 1.1

by Herwin harSens van Welbergen 
chloorrubberharsens@yahoo.com
http://home.student.utwente.nl/h.vanwelbergen/
*/
#ifndef progint
#define progint
#include <dos.h>
//------------------------------------------------------------------------
typedef unsigned char byte;
static const PIC1_base=0x20;
static const PIC2_base=0xa0;
static const byte PIC1_vector_offset=8;
static const byte PIC2_vector_offset=0x70-8;
static const EOI=0x20;
static const OCW1=1;
//------------------------------------------------------------------------
class PIC
{	public:
		void call_oldhandler();
		void write_EOI();
		void set_mask();
		void reset_mask();
		void set(byte irq,void interrupt (*newhandler)(...));
		void reset();
	private:
		byte IRQ;
		byte vector_offset;
		void interrupt(*oldhandler)(...);
		byte old_OCW1;
};
//-------------------------------------------------------------------------
void PIC::call_oldhandler()
{	oldhandler();
}
//-------------------------------------------------------------------------
void PIC::write_EOI()
{	if(IRQ>7)_outp(PIC2_base,EOI);
	_outp(PIC1_base,EOI);
}
//--------------------------------------------------------------------------
void PIC::set_mask()
{   byte value;
	if(IRQ>7)
	{   old_OCW1=_inp(PIC2_base+OCW1);
		value=255-(1<<(IRQ-8));
		_outp(PIC2_base+OCW1,old_OCW1&value);	  //enable irq2 channel
	}
	else
	{   old_OCW1=_inp(PIC1_base+OCW1);
		value=255-(1<<IRQ);
		_outp(PIC1_base+OCW1,old_OCW1&value);
	}
}
//--------------------------------------------------------------------------
void PIC::reset_mask()
{	if(IRQ>7)
	{   _outp(PIC2_base+OCW1,old_OCW1);
	}
	else
	{	_outp(PIC1_base+OCW1,old_OCW1);
	}
}
//---------------------------------------------------------------------------
void PIC::set(byte irq,void interrupt (*newhandler)(...))
{   IRQ=irq;
	if (IRQ>7)
		vector_offset=PIC2_vector_offset;
	else
		vector_offset=PIC1_vector_offset;
	//get old handler
	oldhandler=getvect(IRQ+vector_offset);
	//set new handler
	setvect(IRQ+vector_offset,newhandler);
	//enable interrupt
	set_mask();
}
//-------------------------------------------------------------------------
void PIC::reset()
{   //disable interrupt
	reset_mask();
	//set old handler
	setvect(IRQ+vector_offset,oldhandler);
}
#endif