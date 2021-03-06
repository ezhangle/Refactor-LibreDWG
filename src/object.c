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
 * object.c: adding object functions
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Till Heuschmann
 */

#include "object.h"
#include "decode.h"
#include "logging.h"
#include "dwg.spec"
#include "dwg.h"

static unsigned int loglevel;

//#define DWG_LOGLEVEL loglevel

/* Decode object
 */
void
dwg_decode_add_object(Dwg_Data *dwg, Bit_Chain *dat,
                      long unsigned int address)
{
  long unsigned int previous_address;
  long unsigned int object_address;
  unsigned char previous_bit;
  Dwg_Object *obj;

  /* Keep the previous address
   */
  previous_address = dat->byte;
  previous_bit = dat->bit;

  /* Use the indicated address for the object
   */
  dat->byte = address;
  dat->bit = 0;

  /*
   * Reserve memory space for objects
   */
  if (dwg->num_objects == 0)
    dwg->object = (Dwg_Object *) malloc(sizeof(Dwg_Object));
  else
    dwg->object = (Dwg_Object *) realloc(dwg->object, (dwg->num_objects + 1)
        * sizeof(Dwg_Object));

  if (loglevel)
      LOG_INFO("\n\n======================\nObject number: %lu",
          dwg->num_objects)

  obj = &dwg->object[dwg->num_objects];
  obj->index = dwg->num_objects;
  dwg->num_objects++;

  obj->handle.code = 0;
  obj->handle.size = 0;
  obj->handle.value = 0;

  obj->parent = dwg;
  obj->size = bit_read_MS(dat);
  object_address = dat->byte;
  ktl_lastaddress = dat->byte + obj->size; /* (calculate the bitsize) */
  obj->type = bit_read_BS(dat);

  LOG_INFO(" Type: %d\n", obj->type)

  /* Check the type of the object
   */
  switch (obj->type)
    {
  case DWG_TYPE_TEXT:
    dwg_decode_TEXT(dat, obj);
    break;
  case DWG_TYPE_ATTRIB:
    dwg_decode_ATTRIB(dat, obj);
    break;
  case DWG_TYPE_ATTDEF:
    dwg_decode_ATTDEF(dat, obj);
    break;
  case DWG_TYPE_BLOCK:
    dwg_decode_BLOCK(dat, obj);
    break;
  case DWG_TYPE_ENDBLK:
    dwg_decode_ENDBLK(dat, obj);
    break;
  case DWG_TYPE_SEQEND:
    dwg_decode_SEQEND(dat, obj);
    break;
  case DWG_TYPE_INSERT:
    dwg_decode_INSERT(dat, obj);
    break;
  case DWG_TYPE_MINSERT:
    dwg_decode_MINSERT(dat, obj);
    break;
  case DWG_TYPE_VERTEX_2D:
    dwg_decode_VERTEX_2D(dat, obj);
    break;
  case DWG_TYPE_VERTEX_3D:
    dwg_decode_VERTEX_3D(dat, obj);
    break;
  case DWG_TYPE_VERTEX_MESH:
    dwg_decode_VERTEX_MESH(dat, obj);
    break;
  case DWG_TYPE_VERTEX_PFACE:
    dwg_decode_VERTEX_PFACE(dat, obj);
    break;
  case DWG_TYPE_VERTEX_PFACE_FACE:
    dwg_decode_VERTEX_PFACE_FACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_2D:
    dwg_decode_POLYLINE_2D(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_3D:
    dwg_decode_POLYLINE_3D(dat, obj);
    break;
  case DWG_TYPE_ARC:
    dwg_decode_ARC(dat, obj);
    break;
  case DWG_TYPE_CIRCLE:
    dwg_decode_CIRCLE(dat, obj);
    break;
  case DWG_TYPE_LINE:
    dwg_decode_LINE(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ORDINATE:
    dwg_decode_DIMENSION_ORDINATE(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_LINEAR:
    dwg_decode_DIMENSION_LINEAR(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ALIGNED:
    dwg_decode_DIMENSION_ALIGNED(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ANG3PT:
    dwg_decode_DIMENSION_ANG3PT(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_ANG2LN:
    dwg_decode_DIMENSION_ANG2LN(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_RADIUS:
    dwg_decode_DIMENSION_RADIUS(dat, obj);
    break;
  case DWG_TYPE_DIMENSION_DIAMETER:
    dwg_decode_DIMENSION_DIAMETER(dat, obj);
    break;
  case DWG_TYPE_POINT:
    dwg_decode_POINT(dat, obj);
    break;
  case DWG_TYPE__3DFACE:
    dwg_decode__3DFACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_PFACE:
    dwg_decode_POLYLINE_PFACE(dat, obj);
    break;
  case DWG_TYPE_POLYLINE_MESH:
    dwg_decode_POLYLINE_MESH(dat, obj);
    break;
  case DWG_TYPE_SOLID:
    dwg_decode_SOLID(dat, obj);
    break;
  case DWG_TYPE_TRACE:
    dwg_decode_TRACE(dat, obj);
    break;
  case DWG_TYPE_SHAPE:
    dwg_decode_SHAPE(dat, obj);
    break;
  case DWG_TYPE_VIEWPORT:
    dwg_decode_VIEWPORT(dat, obj);
    break;
  case DWG_TYPE_ELLIPSE:
    dwg_decode_ELLIPSE(dat, obj);
    break;
  case DWG_TYPE_SPLINE:
    dwg_decode_SPLINE(dat, obj);
    break;
  case DWG_TYPE_REGION:
    dwg_decode_REGION(dat, obj);
    break;
  case DWG_TYPE_3DSOLID:
    dwg_decode__3DSOLID(dat, obj);
    break;
  case DWG_TYPE_BODY:
    dwg_decode_BODY(dat, obj);
    break;
  case DWG_TYPE_RAY:
    dwg_decode_RAY(dat, obj);
    break;
  case DWG_TYPE_XLINE:
    dwg_decode_XLINE(dat, obj);
    break;
  case DWG_TYPE_DICTIONARY:
    dwg_decode_DICTIONARY(dat, obj);
    break;
  case DWG_TYPE_MTEXT:
    dwg_decode_MTEXT(dat, obj);
    break;
  case DWG_TYPE_LEADER:
    dwg_decode_LEADER(dat, obj);
    break;
  case DWG_TYPE_TOLERANCE:
    dwg_decode_TOLERANCE(dat, obj);
    break;
  case DWG_TYPE_MLINE:
    dwg_decode_MLINE(dat, obj);
    break;
  case DWG_TYPE_BLOCK_CONTROL:
    dwg_decode_BLOCK_CONTROL(dat, obj);
    break;
  case DWG_TYPE_BLOCK_HEADER:
    dwg_decode_BLOCK_HEADER(dat, obj);
    break;
  case DWG_TYPE_LAYER_CONTROL:
    //set LAYER_CONTROL object - helps keep track of layers
    obj->parent->layer_control = obj;
    dwg_decode_LAYER_CONTROL(dat, obj);
    break;
  case DWG_TYPE_LAYER:
    dwg_decode_LAYER(dat, obj);
    break;
  case DWG_TYPE_SHAPEFILE_CONTROL:
    dwg_decode_SHAPEFILE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_SHAPEFILE:
    dwg_decode_SHAPEFILE(dat, obj);
    break;
  case DWG_TYPE_LTYPE_CONTROL:
    dwg_decode_LTYPE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_LTYPE:
    dwg_decode_LTYPE(dat, obj);
    break;
  case DWG_TYPE_VIEW_CONTROL:
    dwg_decode_VIEW_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VIEW:
    dwg_decode_VIEW(dat, obj);
    break;
  case DWG_TYPE_UCS_CONTROL:
    dwg_decode_UCS_CONTROL(dat, obj);
    break;
  case DWG_TYPE_UCS:
    dwg_decode_UCS(dat, obj);
    break;
  case DWG_TYPE_VPORT_CONTROL:
    dwg_decode_VPORT_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VPORT:
    dwg_decode_VPORT(dat, obj);
    break;
  case DWG_TYPE_APPID_CONTROL:
    dwg_decode_APPID_CONTROL(dat, obj);
    break;
  case DWG_TYPE_APPID:
    dwg_decode_APPID(dat, obj);
    break;
  case DWG_TYPE_DIMSTYLE_CONTROL:
    dwg_decode_DIMSTYLE_CONTROL(dat, obj);
    break;
  case DWG_TYPE_DIMSTYLE:
    dwg_decode_DIMSTYLE(dat, obj);
    break;
  case DWG_TYPE_VP_ENT_HDR_CONTROL:
    dwg_decode_VP_ENT_HDR_CONTROL(dat, obj);
    break;
  case DWG_TYPE_VP_ENT_HDR:
    dwg_decode_VP_ENT_HDR(dat, obj);
    break;
  case DWG_TYPE_GROUP:
    dwg_decode_GROUP(dat, obj);
    break;
  case DWG_TYPE_MLINESTYLE:
    dwg_decode_MLINESTYLE(dat, obj);
    break;
  case DWG_TYPE_LWPLINE:
    dwg_decode_LWPLINE(dat, obj);
    break;
  case DWG_TYPE_HATCH:
    dwg_decode_HATCH(dat, obj);
    break;
  case DWG_TYPE_XRECORD:
    dwg_decode_XRECORD(dat, obj);
    break;
  case DWG_TYPE_PLACEHOLDER:
    dwg_decode_PLACEHOLDER(dat, obj);
    break;
  case DWG_TYPE_LAYOUT:
    dwg_decode_LAYOUT(dat, obj);
    break;
  default:
    if (!dwg_decode_variable_type(dwg, dat, obj))
      {
        LOG_INFO("Object UNKNOWN:\n")

        SINCE(R_2000)
          {
            bit_read_RL(dat);  // skip bitsize
          }

        if (!bit_read_H(dat, &obj->handle))
          {
            LOG_INFO("Object handle: %x.%x.%lx\n", 
              obj->handle.code, obj->handle.size, obj->handle.value)
          }

        obj->supertype = DWG_SUPERTYPE_UNKNOWN;
        obj->tio.unknown = (unsigned char*)malloc(obj->size);
        memcpy(obj->tio.unknown, &dat->chain[object_address], obj->size);
      }
    }

  /*
   if (obj->supertype != DWG_SUPERTYPE_UNKNOWN)
   {
   fprintf (stderr, " Begin address:\t%10lu\n", address);
   fprintf (stderr, " Last address:\t%10lu\tSize: %10lu\n", dat->byte, obj->size);
   fprintf (stderr, "End address:\t%10lu (calculated)\n", address + 2 + obj->size);
   }
   */

  /* Register the previous addresses for return
   */
  dat->byte = previous_address;
  dat->bit = previous_bit;
}

/* OBJECTS */

/* returns 1 if object could be decoded and 0 otherwise
 */
int
dwg_decode_variable_type(Dwg_Data * dwg, Bit_Chain * dat, Dwg_Object* obj)
{
  int i;

  if ((obj->type - 500) > dwg->num_classes)
    return 0;

  i = obj->type - 500;

  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "DICTIONARYVAR"))
    {
      dwg_decode_DICTIONARYVAR(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "ACDBDICTIONARYWDFLT"))
    {
      dwg_decode_DICTIONARYWDLFT(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "HATCH"))
    {
      dwg_decode_HATCH(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IDBUFFER"))
    {
      dwg_decode_IDBUFFER(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGE"))
    {
      dwg_decode_IMAGE(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGEDEF"))
    {
      dwg_decode_IMAGEDEF(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "IMAGEDEF_REACTOR"))
    {
      dwg_decode_IMAGEDEFREACTOR(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LAYER_INDEX"))
    {
      dwg_decode_LAYER_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LAYOUT"))
    {
      dwg_decode_LAYOUT(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "LWPLINE"))
    {
      dwg_decode_LWPLINE(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "OLE2FRAME"))
    {
      dwg_decode_OLE2FRAME(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "ACDBPLACEHOLDER"))
    {
      dwg_decode_PLACEHOLDER(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "RASTERVARIABLES"))
    {
      dwg_decode_RASTERVARIABLES(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SORTENTSTABLE"))
    {
      dwg_decode_SORTENTSTABLE(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SPATIAL_FILTER"))
    {
      dwg_decode_SPATIAL_FILTER(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "SPATIAL_INDEX"))
    {
      dwg_decode_SPATIAL_INDEX(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "VBA_PROJECT"))
    {
//TODO:      dwg_decode_VBA_PROJECT(dat, obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "WIPEOUTVARIABLE"))
    {
//TODO:      dwg_decode_WIPEOUTVARIABLE(dat, obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "XRECORD"))
    {
      dwg_decode_XRECORD(dat, obj);
      return 1;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "DIMASSOC"))
    {
//TODO:      dwg_decode_DIMASSOC(dat, obj);
      return 0;
    }
  if (!strcmp((const char *)dwg->dwg_class[i].dxfname, "MATERIAL"))
    {
//TODO:      dwg_decode_MATERIAL(dat, obj);
      return 0;
    }

  return 0;
}

