#include "AzthObject.h"

AzthObject::AzthObject(Object* /*origin*/)
{
  m_dataUint32.clear();
  m_dataFloat.clear();
}

AzthObject::~AzthObject() {
  m_dataUint32.clear();
  m_dataFloat.clear();
}
