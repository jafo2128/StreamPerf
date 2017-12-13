/*
 *      Copyright (C) 2017-2020 MediaTime
 *      http://media-tm.com (shareviews@sina.com)
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with MediaTime; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 *   original author: Team XBMC
 *       Modified by: shareviews@sina.com (2017-12-XX) without permission
 */

#ifndef MT_CRC32_H_INCLUDE
#define MT_CRC32_H_INCLUDE

#include <string>
#include <stdint.h>

class MTCrc32
{
public:
  MTCrc32();
  void Reset();
  void Compute(const char* buffer, size_t count);
  static uint32_t Compute(const std::string& strValue);
  static uint32_t ComputeFromLowerCase(const std::string& strValue);

  operator uint32_t () const
  {
    return m_crc;
  }

private:
  uint32_t m_crc;
};

#endif /*MT_CRC32_H_INCLUDE*/

