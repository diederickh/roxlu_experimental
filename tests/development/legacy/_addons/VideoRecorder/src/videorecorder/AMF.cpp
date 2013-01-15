#include <videorecorder/AMF.h>

// W R I T E   A M F   I N T O   B U F F E R
// ------------------------------------------------------------------
void AMF::putType(Buffer& buffer, AMFType* v) {
  switch(v->type) {
    case AMF0_TYPE_NUMBER: {
      AMFNumber* c = dynamic_cast<AMFNumber*>(v);
      putNumberAMF0(buffer, *c);
      break;
    };
    case AMF0_TYPE_BOOLEAN: {
      printf("BOOLEAN\n");
      break;
    }
    case AMF0_TYPE_STRING:{
      //printf("STRING");
      AMFString* c = dynamic_cast<AMFString*>(v);
      putStringAMF0(buffer, *c);
      break;
    }
    case AMF0_TYPE_OBJECT: {
      printf("OBJECT");
      break;
    }
    case AMF0_TYPE_MOVIECLIP: {
      printf("MOVIECLIP");
      break;
    }
    case AMF0_TYPE_NULL: {
      printf("NULL");
      break;
    }
    case AMF0_TYPE_UNDEFINED: {
      printf("UNDEFINED");
      break;
    }
    case AMF0_TYPE_REFERENCE: {
      printf("REFERENCE");
      break;
    }
    case AMF0_TYPE_ECMA_ARRAY: {
      //printf("ECMA ARRAY");
      AMFEcmaArray* c = dynamic_cast<AMFEcmaArray*>(v);
      putEcmaArrayAMF0(buffer, *c);
      break;
    }
    case AMF0_TYPE_OBJECT_END: {
      printf("OBJECT_END");
      break;
    }
    case AMF0_TYPE_STRICT_ARRAY: {
      printf("STRICT ARRAY");
      break;
    }
    case AMF0_TYPE_DATE: {
      printf("DATE");
      break;
    }
    case AMF0_TYPE_LONG_STRING: {
      printf("LONG STRING");
      break;
    }
    case AMF0_TYPE_UNSUPPORTED: {
      printf("UNSUPPORTED");
      break;
    }
    case AMF0_TYPE_RECORDSET: {
      printf("RECORDSET");
      break;
    }
    case AMF0_TYPE_XML: {
      printf("XML");
      break;
    }
    case AMF0_TYPE_TYPED_OBJECT: {
      printf("TYPED_OBJECT");
      break;
    }
    default: {
      printf("Cannot put this type: %d\n", v->type);
      break;
    }
  };
}

void AMF::putNumberAMF0(Buffer& buffer, AMFNumber v) {
  buffer.putU8(v.type);
  buffer.putBigEndianDouble(v.getValue());
}

void AMF::putBooleanAMF0(Buffer& buffer, AMFBoolean v) {
  buffer.putU8(v.type);
  buffer.putU8(v.getValue() == false ? 0 : 1);
}

void AMF::putFalseAMF0(Buffer& buffer) {
  AMFBoolean v(false);
  putBooleanAMF0(buffer, v);
}

void AMF::putTrueAMF0(Buffer& buffer) {
  AMFBoolean v(true);
  putBooleanAMF0(buffer, v);
}

// When we write an AMF0 Array we do not write the "string" flag for each 
// element. Therefore the AMFString object has a member "write_tag". When set
// we will prepend the string with a tag identifier. By default the write_tag
// member is set to false. 
void AMF::putStringAMF0(Buffer& buffer, AMFString v) {
  if(v.write_tag) {
    buffer.putU8(v.type);
  }
  buffer.putBigEndianU16(v.getValue().size());
  printf("String: '%s' curr size: %zu\n", v.getValue().c_str(), buffer.size());

  buffer.putBytes((uint8_t*)v.getValue().c_str(), v.getValue().size());
  printf("String: size after: %zu\n---\n", buffer.size());
}

void AMF::putUndefinedAMF0(Buffer& buffer) {
  buffer.putU8(AMF0_TYPE_UNDEFINED);
}

void AMF::putNullAMF0(Buffer& buffer) {
  buffer.putU8(AMF0_TYPE_NULL);
}

void AMF::putEcmaArrayAMF0(Buffer& buffer, AMFEcmaArray v) {
  buffer.putU8(AMF0_TYPE_ECMA_ARRAY);
  buffer.putBigEndianU32(v.elements.size());
  std::vector<AMFType*>::iterator it = v.elements.begin();
  while(it != v.elements.end()) {
    putType(buffer, *it);
    ++it;
  }
  AMFString end_string("", false);
  putStringAMF0(buffer, end_string);
  buffer.putU8(AMF0_TYPE_OBJECT_END);
}



// R E A D   A M F   F R O M   B U F F E R
// ------------------------------------------------------------------
bool AMF::parseBuffer(Buffer& buffer, AMFContainer& container) {
  while(true) {
    AMFType* el = parseType(buffer);
    if(el == NULL) {
      break;
    }
    container.push_back(el);
  }
  return true;
}

AMFType* AMF::parseType(Buffer& buffer) {
  if(buffer.getNumBytesLeftToRead() == 0) {
    return NULL;
  }

  uint8_t type = buffer.getByte();
  switch(type) {

    case AMF0_TYPE_NUMBER: { return getNumberAMF0(buffer); break; }
    case AMF0_TYPE_BOOLEAN: { return getBooleanAMF0(buffer);	break; }
    case AMF0_TYPE_STRING: { return getStringAMF0(buffer); break; }

    case AMF0_TYPE_OBJECT: {
      printf("AMF OBJECT\n");
      ::exit(0);
      break;
    }

    case AMF0_TYPE_MOVIECLIP: {
      printf("AMF MOVIECLIP\n");
      ::exit(0);
      break;
    }

    case AMF0_TYPE_UNDEFINED: {
      printf("AMF UNDEFINED\n");
      ::exit(0);
      break;
    }

    case AMF0_TYPE_REFERENCE: {
      printf("AMF REFERENCE\n");
      ::exit(0);
      break;
    }
			
    case AMF0_TYPE_ECMA_ARRAY: { return getEcmaArrayAMF0(buffer); break; }
    case AMF0_TYPE_OBJECT_END: { return getObjectEndAMF0(buffer); break;	}

    case AMF0_TYPE_STRICT_ARRAY: {
      printf("AMF STRICT ARRAY\n");
      ::exit(0);
      break;
    }

    case AMF0_TYPE_DATE: {
      printf("AMF DATE\n");
      ::exit(0);
      break;
    }

    case AMF0_TYPE_LONG_STRING: {
      printf("AMF LONG STRING\n");
      ::exit(0);
      break;
    }

    case AMF0_TYPE_UNSUPPORTED: {
      printf("AMF UNSUPPORTED\n");
      ::exit(0);
      break;
    }

    case AMF0_TYPE_RECORDSET: {
      printf("AMF RECORDSET\n");
      ::exit(0);
      break;
    }
			
    case AMF0_TYPE_XML: {
      printf("AMF XML\n");
      ::exit(0);
      break;
    }

    case AMF0_TYPE_TYPED_OBJECT: {
      printf("AMF OBJECT\n");
      ::exit(0);
      break;
    }
    default: break;
  }
  return NULL;
}

AMFNumber* AMF::getNumberAMF0(Buffer& buffer) {
  double d = buffer.getBigEndianDouble();
  AMFNumber* element = new AMFNumber(d);
  return element;
}

AMFBoolean* AMF::getBooleanAMF0(Buffer& buffer) {
  uint8_t val = buffer.getByte();
  AMFBoolean* element = new AMFBoolean((val != 0));
  return element;
}

AMFString* AMF::getStringAMF0(Buffer& buffer) {
  uint16_t size = buffer.getBigEndianU16();
  std::string value = "";
  for(int i = 0; i < size; ++i) {
    value.push_back(buffer.getByte());
  }
  return new AMFString(value);
}

AMFObjectEnd* AMF::getObjectEndAMF0(Buffer& buffer) {
  return new AMFObjectEnd();
}

AMFLongString* AMF::getLongStringAMF0(Buffer& buffer) {
  return NULL;
}

AMFEcmaArray* AMF::getEcmaArrayAMF0(Buffer& buffer) {
  uint32_t count = buffer.getBigEndianU32();
  //printf("Number of elements in array: %d\n", count);
  AMFEcmaArray* array = new AMFEcmaArray();

  while(true) {
    AMFString* array_name = getStringAMF0(buffer);
    AMFType* array_value = parseType(buffer);
    array->push_back(array_name);
    array->push_back(array_value);
    if(array_value->getType() == AMF0_TYPE_OBJECT_END) {
      break;
    }
  }
  return array;
}
