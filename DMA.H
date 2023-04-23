/*
a Herwin production '98
version 2.02

by Herwin harSens van Welbergen
chloorrubberharsens@yahoo.com
http://home.student.utwente.nl/h.vanwelbergen/
*/
#ifndef dmacheck
#define dmacheck
#include<conio.h>
#include<dos.h>
#define LO(n) (n&0x00ff)
#define HI(n) ((n&0xff00)>>8)
typedef unsigned char byte;
//-------------------------------------------------------------------------

//mask_port:
const int	clearmask=0,
		setmask=4;
const int	chan0=1,
		chan1=2,
		chan2=4,
		chan4=8;

//mode_port:
const int  verify=0,
		write___=4,
		read___=8,
		single_cycle=0,
		auto_init=16,
		adress_inc=0,
		adress_dec=32,
		demand_mode=0,
		signal_mode=64,
		block_mode=128,
		cascade_mode=192;

//used with command port
const int	extended_write=32,
		late_write=0,
		rotating=16,
		fixed=0,
		compressed=8,
		uncompressed=0,
		enable_control=4,
		disable_control=0,
		enable_ahold=2,
		disable_ahold=0,
		enable_mem_mem=1,
		disable_mem_mem=0;
//request port:
const int	set_request=4,
		reset_request=0;
//-------------------------------------------------------------------------
class DMA
{   public:
		char *bufp;
		char request_pending[4];
		char terminal_count[4];
		void set_channel(const char);
		void make_block(const void*,const unsigned);
		void set_mask(byte mask);
		void set_mode(byte mode);
		void clear();
		void set_page();
		void set_address();
		void set_length();
		void stop();
		void reset_all();
		unsigned get_length();
		void set_command(byte command);
		void get_status();
		void set_request(byte req);
		void start(unsigned char,void *,unsigned,byte mode);
		void *make_buffer(unsigned &netto_size);
		void set_more_mask(byte);
	private:
		unsigned char page_port;
		char address_port;
		char length_port;
		char channel;
		unsigned page;
		unsigned offset;
		unsigned length;
		byte chip;
};
//-----------------------------------------------------------------------------
void DMA::set_channel(const char n)
{	channel=n;
	switch (channel)
	{	case 0:page_port=0x87;address_port=0;length_port=1;break;
		case 1:page_port=0x83;address_port=2;length_port=3;break;
		case 2:page_port=0x81;address_port=4;length_port=5;break;
		case 3:page_port=0x82;address_port=6;length_port=7;break;
		case 4:page_port=0x8f;address_port=0xc0;length_port=0xc2;break;
		case 5:page_port=0x8b;address_port=0xc4;length_port=0xc6;break;
		case 6:page_port=0x89;address_port=0xc8;length_port=0xca;break;
		case 7:page_port=0x8a;address_port=0xcc;length_port=0xce;break;
	}
}
//------------------------------------------------------------------------
void DMA::make_block(const void *pointer,const unsigned size)
{	offset=(FP_SEG(pointer)<<4)+FP_OFF(pointer);
	page=(FP_SEG(pointer)+(FP_OFF(pointer)>>4))>>12;
	length=size;
}
//------------------------------------------------------------------------
void DMA::set_command(byte command)
{	_outp(0x8,command);
}
//-------------------------------------------------------------------------
void DMA::get_status()
{	unsigned char status=_inp(0x08);
	int x=64;
	for(int n=0;n<4;n++)
	{   x*=2;
		request_pending[n]=status&x;//1:channel=waiting
	}
	x=2;
	for(n=0;n<4;n++)
	{   x*=2;
		terminal_count[n]=status&x;//1:channel=done
	}

}
//-------------------------------------------------------------------------
void DMA::set_request(byte req)
{	_outp(0x09,req);
}
//------------------------------------------------------------------------
void DMA::set_mask(byte masker)
{	_outp(0xa,masker|channel);
}
//------------------------------------------------------------------------
void DMA::set_mode(byte mode)
{	_outp(0xb,mode|channel);
}
//------------------------------------------------------------------------
void DMA::clear()
{	_outp(0xc,0);
}
//------------------------------------------------------------------------
void DMA::set_page()
{	_outp(page_port,page);
}
//------------------------------------------------------------------------
void DMA::set_address()
{	_outp(address_port,LO(offset));
	_outp(address_port,HI(offset));
}
//------------------------------------------------------------------------
void DMA::set_length()
{	_outp(length_port,LO(length));
	_outp(length_port,HI(length));
}
//-------------------------------------------------------------------------
void DMA::stop()
{	set_mask(setmask);
	clear();
	set_mask(clearmask);
}
//------------------------------------------------------------------------
unsigned DMA::get_length()
{ 	unsigned remaining_length;
	disable();
	clear();
	remaining_length=_inp(length_port);
	remaining_length+=_inp(length_port)<<8;
	enable();
	return length-remaining_length;
}
//-------------------------------------------------------------------------
void *DMA::make_buffer(unsigned &netto_size)
{   char *newpnt;
	unsigned bruto_size=netto_size/3*4;
	bufp=new char [bruto_size];
	unsigned off=(FP_SEG(bufp)<<4)+FP_OFF(bufp);
	unsigned pag=(FP_SEG(bufp)+(FP_OFF(bufp)>>4))>>12;

	if ((65536L-off)>=bruto_size/2)
	{  //pointer allign is OK, calculate size
		netto_size=65536L-off;
		newpnt=bufp;
	}
	else
	{  //pointer is short under a page break, put
		//pointer on the page break and calculate size
		pag++;
		netto_size=bruto_size+off-65536L;
		void *newpnt=MKP(pag*0x1000,0);
	}
	return newpnt;
}
//-------------------------------------------------------------------------
void DMA::reset_all()
{	_outp(0xd,0);
}
//-------------------------------------------------------------------------
void DMA::start(unsigned char chan,void *buffer,unsigned size,byte mode)
{	set_channel(chan);
	make_block(buffer,size);
	disable();
	set_mask(setmask);
	clear();
	set_mode(mode);
	set_page();
	set_address();
	set_length();
	set_mask(clearmask);
	enable();
}
//-------------------------------------------------------------------------
void DMA::set_more_mask(byte mm)
{	_outp(0xF,mm);
}
#endif


