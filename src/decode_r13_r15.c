/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * decode_r13_r15.c: decoding functions
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Till Heuschmann
 */

#include "classes.h"
#include "decode_r13_r15.h"
#include "handle.h"
#include "header.h"
#include "logging.h"
#include "resolve_pointers.h"
#include "section_locate.h"


/* Read Unknown Section 
 */
void
read_section_unknown(Bit_Chain *dat, Dwg_Data *dwg)
{
  if (dwg->header.num_sections == 6)
    {
      LOG_TRACE("\n UNKNOWN 1: %8X\n",
                (unsigned int) dwg->header.section[5].address)
      LOG_TRACE(" UNKNOWN 1 (end): %8X\n",
                (unsigned int) (dwg->header.section[5].address
                + dwg->header.section[5].size))

      dat->byte = dwg->header.section[5].address;
      dwg->unknown1.size = DWG_UNKNOWN1_SIZE;
      dwg->unknown1.byte = dwg->unknown1.bit = 0;
      dwg->unknown1.chain = (unsigned char*) malloc(dwg->unknown1.size);
      memcpy(dwg->unknown1.chain, &dat->chain[dat->byte], 
             dwg->unknown1.size);

      //bit_explore_chain ((Bit_Chain *) &dwg->unknown1, dwg->unknown1.size);
      //bit_print ((Bit_Chain *) &dwg->unknown1, dwg->unknown1.size);
    }
}

/* Read Picture Section
 */
void
read_section_picture(Bit_Chain *dat, Dwg_Data *dwg)
{
  if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_BEGIN)))
    {
      unsigned long int start_address;
      dat->bit = 0;
      start_address = dat->byte;

      LOG_TRACE("\n PICTURE: %8X\n",
                (unsigned int) start_address - 16)

      if (bit_search_sentinel(dat, dwg_sentinel(DWG_SENTINEL_PICTURE_END)))
        {
          LOG_TRACE(" PICTURE (end): %8X\n",
                    (unsigned int) dat->byte)

          dwg->picture.size = (dat->byte - 16) - start_address;
          dwg->picture.chain = (unsigned char *) malloc(dwg->picture.size);
          memcpy(dwg->picture.chain, &dat->chain[start_address],
                 dwg->picture.size);
        }
      else
        dwg->picture.size = 0;
    }
}

/* Read Second Header
 */
void
read_section_second_header(Bit_Chain *dat, Dwg_Data *dwg)
{
  if (bit_search_sentinel(dat, 
      dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_BEGIN)))
    {
      long unsigned int ckr;
      long unsigned int pvz;
      long unsigned int pvzadr;
      unsigned char sig, sig2;
      unsigned int i, j;

      LOG_INFO("\n Second Header: %8X\n",
               (unsigned int) dat->byte-16)

      pvzadr = dat->byte;
      pvz = bit_read_RL(dat);
      //LOG_TRACE("Size: %lu\n", pvz)

      pvz = bit_read_BL(dat);
      //LOG_TRACE("Begin address: %8X\n", pvz)

      //LOG_TRACE("AC1015?: ")
      for (i = 0; i < 6; i++)
        {
          sig = bit_read_RC(dat);
          //LOG_TRACE("%c", sig >= ' ' && sig < 128 ? sig : '.')
        }

      //LOG_TRACE("Null?:")
      for (i = 0; i < 5; i++) // 6 if is older...
        {
          sig = bit_read_RC(dat);
          //LOG_TRACE(" 0x%02X", sig)
        }

      //LOG_TRACE"4 null bits?: ")
      for (i = 0; i < 4; i++)
        {
          sig = bit_read_B(dat);
          //LOG_TRACE(" %c", sig ? '1' : '0')
        }

      //LOG_TRACE(stderr, "Chain?: ")
      for (i = 0; i < 6; i++)
        {
          dwg->second_header.unknown[i] = bit_read_RC(dat);
          //LOG_TRACE(" 0x%02X", dwg->second_header.unknown[i])
        }
      if (dwg->second_header.unknown[3] != 0x78
          || dwg->second_header.unknown[5] != 0x06)
        sig = bit_read_RC(dat); // To compensate in the event of a contingent
                                // additional zero not read previously

      //puts("");
      for (i = 0; i < 6; i++)
        {
          sig = bit_read_RC(dat);
          //if (loglevel) fprintf (stderr, "[%u]\n", sig);
          pvz = bit_read_BL(dat);
          //if (loglevel) fprintf (stderr, " Address: %8X\n", pvz);
          pvz = bit_read_BL(dat);
          //if (loglevel) fprintf (stderr, "  Size: %8X\n", pvz);
        }

      bit_read_BS(dat);
      //if (loglevel) fprintf (stderr, "\n14 --------------");
      for (i = 0; i < 14; i++)
        {
          sig2 = bit_read_RC(dat);
          dwg->second_header.handlerik[i].size = sig2;
          //if (loglevel) fprintf (stderr, "\nSize: %u\n", sig2);
          sig = bit_read_RC(dat);
          //if (loglevel) fprintf (stderr, "\t[%u]\n", sig);
          //if (loglevel) fprintf (stderr, "\tChain:");
          for (j = 0; j < sig2; j++)
            {
              sig = bit_read_RC(dat);
              dwg->second_header.handlerik[i].chain[j] = sig;
              //if (loglevel) fprintf (stderr, " %02X", sig);
            }
        }

      // Check CRC-on
      ckr = bit_read_CRC(dat);
      /*
       puts ("");
       for (i = 0; i != 0xFFFF; i++)
       {
       dat->byte -= 2;
       bit_write_CRC (dat, pvzadr, i);
       dat->byte -= 2;
       ckr2 = bit_read_CRC (dat);
       if (ckr == ckr2)
       {
         if (loglevel) 
           fprintf (stderr, "Read: %X\nCreated: %X\t SEMO: %02X\n", ckr, ckr2, i);
         break;
       }
       }
       if (loglevel) {
       fprintf (stderr, " Garbage 1: %08X\n", bit_read_RL (dat));
       fprintf (stderr, " Garbage 2: %08X\n", bit_read_RL (dat));
       }
       */

      if (bit_search_sentinel(dat, 
          dwg_sentinel(DWG_SENTINEL_SECOND_HEADER_END)))
        LOG_INFO(" Second Header (end): %8X\n",
                 (unsigned int) dat->byte)
    }
}

/* Read Section Measurement
 */
void
read_section_measurement(Bit_Chain *dat, Dwg_Data *dwg)
{
  LOG_INFO("\n Unknown 2: %8X\n",
           (unsigned int) dwg->header.section[4].address)
  LOG_INFO(" Unknown 2 (end): %8X\n",
           (unsigned int) (dwg->header.section[4].address
           + dwg->header.section[4].size))

  dat->byte = dwg->header.section[4].address;
  dat->bit = 0;
  dwg->measurement = bit_read_RL(dat);

  LOG_TRACE(" Size bytes :\t%lu\n", dat->size)

  //step II of handles parsing: resolve pointers from handle value
  //XXX: move this somewhere else
  LOG_TRACE("\n\nResolving pointers from ObjectRef vector.\n")
  resolve_objectref_vector(dwg);
  LOG_TRACE("\n")
}

/* Decode R13-R15 
 */
int
decode_R13_R15(Bit_Chain *dat, Dwg_Data *dwg)
{
  unsigned char sig;
  long unsigned int pvz;
  unsigned int i;

  // Still unknown values: 6 'zeroes' and a 'one'
  dat->byte = 0x06;
  LOG_TRACE("Still unknown values: 6 'zeroes' and a 'one': ")
  for (i = 0; i < 7; i++)
    {
      sig = bit_read_RC(dat);
      LOG_TRACE("0x%02X", sig)
    }
  LOG_TRACE("\n")

  /* Image Seeker */
  pvz = bit_read_RL(dat);
  LOG_TRACE("Image seeker: 0x%08X\n", (unsigned int) pvz)

  /* unknown */
  sig = bit_read_RC(dat);
  LOG_INFO("Version: %u\n", sig);
  sig = bit_read_RC(dat);
  LOG_INFO("Release: %u\n", sig);

  /* Codepage */
  dat->byte = 0x13;
  dwg->header.codepage = bit_read_RS(dat);
  LOG_INFO("Codepage: %u\n", dwg->header.codepage);

  /* Section Locator Records */
  dat->byte = 0x15;
  read_R13_R15_section_locate(dat, dwg);

  /* Unknown section 1*/
  read_section_unknown(dat, dwg);

  /* Picture (Pre-R13C3) */
  read_section_picture(dat, dwg);

  /* Header Variables */
  read_R13_R15_section_header(dat, dwg);

  /* Classes */
  read_R13_R15_section_classes(dat, dwg);

  /* Object-map */
  read_R13_R15_section_object_map(dat, dwg);

  /* Second header */
  read_section_second_header(dat, dwg);

  /* Section Measurement */
  read_section_measurement(dat, dwg);

  return 0;
}
