/***********************************************************************
* ftermfreebsd-test.cpp - FTermFreeBSD unit tests                      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <limits>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <unistd.h>

#include <conemu.h>
#include <final/final.h>

namespace test
{

//----------------------------------------------------------------------
// class FSystemTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FSystemTest : public finalcut::FSystem
{
  public:
    // Constructor
    FSystemTest();

    // Destructor
    virtual ~FSystemTest();

    // Methods
    virtual uChar    inPortByte (uShort) override;
    virtual void     outPortByte (uChar, uShort) override;
    virtual int      isTTY (int) override;
    virtual int      ioctl (int, uLong, ...) override;
    virtual int      open (const char*, int, ...) override;
    virtual int      close (int) override;
    virtual FILE*    fopen (const char*, const char*) override;
    virtual int      fclose (FILE*) override;
    virtual int      putchar (int) override;
    virtual int      tputs (const char*, int, int (*)(int)) override;
    virtual uid_t    getuid() override;
    std::string&     getCharacters();
    int&             getCursorType();

  private:
    // Data Members
    std::string characters;
    int cursor_type = 0;
    static struct keymap_t keymap;
    static struct keymap_t terminal_keymap;
};
#pragma pack(pop)

// private Data Member of FSystemTest
//----------------------------------------------------------------------
struct keymap_t FSystemTest::keymap =
{
  109, // Number of keys
  {
    //                     map                          spcl  flag
    //------------------------------------------------  ----  ----
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0xff, 0x00 },
    { {0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x86, 0x1b}, 0x02, 0x00 },
    { {0x31, 0x21, 0x00, 0x00, 0x31, 0x21, 0x00, 0x00}, 0x33, 0x00 },
    { {0x32, 0x22, 0x00, 0x00, 0xb2, 0xb2, 0x00, 0x00}, 0x33, 0x00 },
    { {0x33, 0xa7, 0x00, 0x00, 0xb3, 0xb3, 0x00, 0x00}, 0x33, 0x00 },
    { {0x34, 0x24, 0x00, 0x00, 0x34, 0x24, 0x00, 0x00}, 0x33, 0x00 },
    { {0x35, 0x25, 0x00, 0x00, 0x35, 0x25, 0x00, 0x00}, 0x33, 0x00 },
    { {0x36, 0x26, 0x00, 0x00, 0x36, 0x26, 0x00, 0x00}, 0x33, 0x00 },
    { {0x37, 0x2f, 0x00, 0x00, 0x7b, 0x7b, 0x00, 0x00}, 0x33, 0x00 },
    { {0x38, 0x28, 0x1b, 0x1b, 0x5b, 0x5b, 0x1b, 0x1b}, 0x00, 0x00 },
    { {0x39, 0x29, 0x1d, 0x1d, 0x5d, 0x5d, 0x1d, 0x1d}, 0x00, 0x00 },
    { {0x30, 0x3d, 0x00, 0x00, 0x7d, 0x7d, 0x00, 0x00}, 0x33, 0x00 },
    { {0xdf, 0x3f, 0x1c, 0x1c, 0x5c, 0x5c, 0x1c, 0x1c}, 0x00, 0x00 },
    { {0x27, 0x60, 0x00, 0x00, 0xb3, 0xb4, 0x00, 0x00}, 0x33, 0x00 },
    { {0x08, 0x08, 0x7f, 0x7f, 0x08, 0x08, 0x7f, 0x7f}, 0x00, 0x00 },
    { {0x09, 0x08, 0x00, 0x00, 0x09, 0x08, 0x00, 0x00}, 0x77, 0x00 },
    { {0x71, 0x51, 0x11, 0x11, 0x40, 0x40, 0x00, 0x00}, 0x00, 0x01 },
    { {0x77, 0x57, 0x17, 0x17, 0x77, 0x57, 0x17, 0x17}, 0x00, 0x01 },
    { {0x65, 0x45, 0x05, 0x05, 0x20ac, 0x45, 0x05, 0x05}, 0x00, 0x01 },
    { {0x72, 0x52, 0x12, 0x12, 0x72, 0x52, 0x12, 0x12}, 0x00, 0x01 },
    { {0x74, 0x54, 0x14, 0x14, 0x74, 0x54, 0x14, 0x14}, 0x00, 0x01 },
    { {0x7a, 0x5a, 0x1a, 0x1a, 0x7a, 0x5a, 0x1a, 0x1a}, 0x00, 0x01 },
    { {0x75, 0x55, 0x15, 0x15, 0x75, 0x55, 0x15, 0x15}, 0x00, 0x01 },
    { {0x69, 0x49, 0x09, 0x09, 0x69, 0x49, 0x09, 0x09}, 0x00, 0x01 },
    { {0x6f, 0x4f, 0x0f, 0x0f, 0x6f, 0x4f, 0x0f, 0x0f}, 0x00, 0x01 },
    { {0x70, 0x50, 0x10, 0x10, 0x70, 0x50, 0x10, 0x10}, 0x00, 0x01 },
    { {0xfc, 0xdc, 0x00, 0x00, 0xfc, 0xdc, 0x1b, 0x00}, 0x31, 0x01 },
    { {0x2b, 0x2a, 0x00, 0x00, 0x7e, 0x7e, 0x00, 0x00}, 0x33, 0x00 },
    { {0x0d, 0x0d, 0x0a, 0x0a, 0x0d, 0x0d, 0x0a, 0x0a}, 0x00, 0x00 },
    { {0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09}, 0xff, 0x00 },
    { {0x61, 0x41, 0x01, 0x01, 0x61, 0x41, 0x01, 0x01}, 0x00, 0x01 },
    { {0x73, 0x53, 0x13, 0x13, 0x73, 0x53, 0x13, 0x13}, 0x00, 0x01 },
    { {0x64, 0x44, 0x04, 0x04, 0x64, 0x44, 0x04, 0x04}, 0x00, 0x01 },
    { {0x66, 0x46, 0x06, 0x06, 0x66, 0x46, 0x06, 0x06}, 0x00, 0x01 },
    { {0x67, 0x47, 0x07, 0x07, 0x67, 0x47, 0x07, 0x07}, 0x00, 0x01 },
    { {0x68, 0x48, 0x08, 0x08, 0x68, 0x48, 0x08, 0x08}, 0x00, 0x01 },
    { {0x6a, 0x4a, 0x0a, 0x0a, 0x6a, 0x4a, 0x0a, 0x0a}, 0x00, 0x01 },
    { {0x6b, 0x4b, 0x0b, 0x0b, 0x6b, 0x4b, 0x0b, 0x0b}, 0x00, 0x01 },
    { {0x6c, 0x4c, 0x0c, 0x0c, 0x6c, 0x4c, 0x0c, 0x0c}, 0x00, 0x01 },
    { {0xf6, 0xd6, 0x00, 0x00, 0xf6, 0xd6, 0x00, 0x00}, 0x33, 0x01 },
    { {0xe4, 0xc4, 0x00, 0x00, 0xe4, 0xc4, 0x00, 0x00}, 0x33, 0x01 },
    { {0x5e, 0xb0, 0x1e, 0x1e, 0x5e, 0xb0, 0x1e, 0x1e}, 0x00, 0x00 },
    { {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02}, 0xff, 0x00 },
    { {0x23, 0x27, 0x00, 0x00, 0x23, 0x27, 0x00, 0x00}, 0x33, 0x00 },
    { {0x79, 0x59, 0x19, 0x19, 0x79, 0x59, 0x19, 0x19}, 0x00, 0x01 },
    { {0x78, 0x58, 0x18, 0x18, 0x78, 0x58, 0x18, 0x18}, 0x00, 0x01 },
    { {0x63, 0x43, 0x03, 0x03, 0xa2, 0x43, 0x03, 0x03}, 0x00, 0x01 },
    { {0x76, 0x56, 0x16, 0x16, 0x76, 0x56, 0x16, 0x16}, 0x00, 0x01 },
    { {0x62, 0x42, 0x02, 0x02, 0x62, 0x42, 0x02, 0x02}, 0x00, 0x01 },
    { {0x6e, 0x4e, 0x0e, 0x0e, 0x6e, 0x4e, 0x0e, 0x0e}, 0x00, 0x01 },
    { {0x6d, 0x4d, 0x0d, 0x0d, 0xb5, 0xb5, 0x0d, 0x0d}, 0x00, 0x01 },
    { {0x2c, 0x3b, 0x00, 0x00, 0x2c, 0x3b, 0x00, 0x00}, 0x33, 0x00 },
    { {0x2e, 0x3a, 0x00, 0x00, 0x2e, 0x3a, 0x00, 0x00}, 0x33, 0x00 },
    { {0x2d, 0x5f, 0x1f, 0x1f, 0x2d, 0x5f, 0x1f, 0x1f}, 0x00, 0x00 },
    { {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03}, 0xff, 0x00 },
    { {0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a}, 0x00, 0x00 },
    { {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07}, 0xff, 0x00 },
    { {0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x87, 0x20}, 0x02, 0x00 },
    { {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}, 0xff, 0x00 },
    { {0x1b, 0x27, 0x33, 0x3f, 0x0b, 0x15, 0x0b, 0x15}, 0xff, 0x00 },
    { {0x1c, 0x28, 0x34, 0x40, 0x0c, 0x16, 0x0c, 0x16}, 0xff, 0x00 },
    { {0x1d, 0x29, 0x35, 0x41, 0x0d, 0x17, 0x0d, 0x17}, 0xff, 0x00 },
    { {0x1e, 0x2a, 0x36, 0x42, 0x0e, 0x18, 0x0e, 0x18}, 0xff, 0x00 },
    { {0x1f, 0x2b, 0x37, 0x43, 0x0f, 0x19, 0x0f, 0x19}, 0xff, 0x00 },
    { {0x20, 0x2c, 0x38, 0x44, 0x10, 0x1a, 0x10, 0x1a}, 0xff, 0x00 },
    { {0x21, 0x2d, 0x39, 0x45, 0x11, 0x11, 0x11, 0x11}, 0xff, 0x00 },
    { {0x22, 0x2e, 0x3a, 0x46, 0x12, 0x12, 0x12, 0x12}, 0xff, 0x00 },
    { {0x23, 0x2f, 0x3b, 0x47, 0x13, 0x13, 0x13, 0x13}, 0xff, 0x00 },
    { {0x24, 0x30, 0x3c, 0x48, 0x14, 0x14, 0x14, 0x14}, 0xff, 0x00 },
    { {0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05}, 0xff, 0x00 },
    { {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}, 0xff, 0x00 },
    { {0x4b, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37}, 0x80, 0x02 },
    { {0x4c, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38}, 0x80, 0x02 },
    { {0x4d, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39}, 0x80, 0x02 },
    { {0x4e, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d}, 0x80, 0x02 },
    { {0x4f, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34}, 0x80, 0x02 },
    { {0x50, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35, 0x35}, 0x80, 0x02 },
    { {0x51, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36}, 0x80, 0x02 },
    { {0x52, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b}, 0x80, 0x02 },
    { {0x53, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31}, 0x80, 0x02 },
    { {0x54, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32}, 0x80, 0x02 },
    { {0x55, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33}, 0x80, 0x02 },
    { {0x56, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30}, 0x80, 0x02 },
    { {0x7f, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x85, 0x85}, 0x03, 0x02 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0xff, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0xff, 0x00 },
    { {0x3c, 0x3e, 0x00, 0x00, 0x7c, 0xa6, 0x00, 0x00}, 0x33, 0x00 },
    { {0x25, 0x31, 0x3d, 0x49, 0x15, 0x15, 0x15, 0x15}, 0xff, 0x00 },
    { {0x26, 0x32, 0x3e, 0x4a, 0x16, 0x16, 0x16, 0x16}, 0xff, 0x00 },
    { {0x0d, 0x0d, 0x0a, 0x0a, 0x0d, 0x0d, 0x0a, 0x0a}, 0x00, 0x00 },
    { {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}, 0xff, 0x00 },
    { {0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f, 0x2f}, 0x00, 0x02 },
    { {0x0a, 0x99, 0x86, 0x86, 0x00, 0x00, 0x00, 0x00}, 0xff, 0x00 },
    { {0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81}, 0xff, 0x00 },
    { {0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b, 0x4b}, 0xff, 0x00 },
    { {0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4c}, 0xff, 0x00 },
    { {0x4d, 0x4d, 0x4d, 0x4d, 0x4d, 0x4d, 0x4d, 0x4d}, 0xff, 0x00 },
    { {0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f}, 0xff, 0x00 },
    { {0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51}, 0xff, 0x00 },
    { {0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53}, 0xff, 0x00 },
    { {0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54}, 0xff, 0x00 },
    { {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, 0xff, 0x00 },
    { {0x56, 0xa3, 0x56, 0x56, 0x56, 0x56, 0x56, 0x56}, 0xff, 0x00 },
    { {0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x85, 0x57}, 0xff, 0x00 },
    { {0x06, 0x88, 0x06, 0x88, 0x87, 0x00, 0x87, 0x00}, 0xff, 0x00 },
    { {0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58, 0x58}, 0xff, 0x00 },
    { {0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59}, 0xff, 0x00 },
    { {0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a}, 0xff, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0xff, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 },
    { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 0x00, 0x00 }
    //  |     |     |     |     |     |     |     |
    //  |     |     |     |     |     |     |     `--- 0: Shift-Ctrl-Alt
    //  |     |     |     |     |     |     `--------- 1: Ctrl-Alt
    //  |     |     |     |     |     `--------------- 2: Shift-Alt
    //  |     |     |     |     `--------------------- 3: Alt
    //  |     |     |     `--------------------------- 4: Shift-Ctrl
    //  |     |     `--------------------------------- 5: Ctrl
    //  |     `--------------------------------------- 6: Shift
    //  `--------------------------------------------- 7: Base
    //                                                 |
    //                                                 `- spcl bit
    //
    // spcl = A special treatment key (bits correspond to the map field)
    // flag = 0 -> 'O' = Caps lock + num lock are ignored
    // flag = 1 -> 'C' = Caps lock affects the key
    // flag = 2 -> 'N' = Num lock affects the key
  }
};

// static class attributes
//----------------------------------------------------------------------
struct keymap_t  FSystemTest::terminal_keymap{};

// constructors and destructor
//----------------------------------------------------------------------
FSystemTest::FSystemTest()  // constructor
{
}

//----------------------------------------------------------------------
FSystemTest::~FSystemTest()  // destructor
{
}


// public methods of FSystemTest
//----------------------------------------------------------------------
uChar FSystemTest::inPortByte (uShort)
{
  return 0;
}

//----------------------------------------------------------------------
void FSystemTest::outPortByte (uChar, uShort)
{
}

//----------------------------------------------------------------------
int FSystemTest::isTTY (int fd)
{
  std::cerr << "Call: isatty (fd=" << fd << ")\n";
  return 1;
}

//----------------------------------------------------------------------
int FSystemTest::ioctl (int fd, uLong request, ...)
{
  va_list args;
  void* argp;
  std::string req_string;
  int ret_val = -1;

  va_start (args, request);
  argp = va_arg (args, void*);

  switch ( request )
  {
    case CONS_CURSORTYPE:
    {
      req_string = "CONS_CURSORTYPE";
      constexpr int blink_cursor  = int(1 << 0);
      constexpr int char_cursor   = int(1 << 1);
      constexpr int hidden_cursor = int(1 << 2);
      constexpr int reset_cursor  = int(1 << 30);
      constexpr int cursor_attrs  = int( blink_cursor \
                                       | char_cursor \
                                       | hidden_cursor );
      int* cur_flags = static_cast<int*>(argp);
      *cur_flags = *cur_flags & cursor_attrs;

      if ( *cur_flags & reset_cursor )
        cursor_type = 0;
      else
        cursor_type = *cur_flags;

      ret_val = 0;
      break;
    }

    case GIO_KEYMAP:
    {
      req_string = "GIO_KEYMAP";
      keymap_t* kmap = static_cast<keymap_t*>(argp);

      // Sets the default keymap of the terminal on the first call
      if ( terminal_keymap.n_keys == 0 )
      {
        terminal_keymap.n_keys = keymap.n_keys;
        std::memcpy (terminal_keymap.key, &keymap.key, sizeof(keymap.key));
      }

      kmap->n_keys = terminal_keymap.n_keys;
      std::memcpy (kmap->key, terminal_keymap.key, sizeof(keymap.key));
      ret_val = 0;
      break;
    }

    case PIO_KEYMAP:
    {
      req_string = "PIO_KEYMAP";
      keymap_t* kmap = static_cast<keymap_t*>(argp);
      std::memcpy (terminal_keymap.key, kmap->key, sizeof(keymap.key));
      ret_val = 0;
      break;
    }

    case TIOCGWINSZ:
      req_string = "TIOCGWINSZ";
      struct winsize* win_size = static_cast<winsize*>(argp);
      win_size->ws_col = 80;
      win_size->ws_row = 25;
      ret_val = 0;
      break;
  }

  va_end (args);

  std::cerr << "Call: ioctl (fd=" << fd
            << ", request=" << req_string
            << "(0x" << std::hex << request << ")"
            << ", argp=" << argp << std::dec << ")\n";
  return ret_val;
}

//----------------------------------------------------------------------
int FSystemTest::open (const char* pathname, int flags, ...)
{
  va_list args;
  va_start (args, flags);
  mode_t mode = static_cast<mode_t>(va_arg (args, int));
  va_end (args);

  std::cerr << "Call: open (pathname=\"" << pathname
            << "\", flags=" << flags
            << ", mode=" << mode << ")\n";

  return 0;
}

//----------------------------------------------------------------------
int FSystemTest::close (int fildes)
{
  std::cerr << "Call: close (fildes=" << fildes << ")\n";
  return 0;
}

//----------------------------------------------------------------------
FILE* FSystemTest::fopen (const char* path, const char* mode)
{
  std::cerr << "Call: fopen (path=" << path
            << ", mode=" << mode << ")\n";
  return 0;
}

//----------------------------------------------------------------------
int FSystemTest::fclose (FILE* fp)
{
  std::cerr << "Call: fclose (fp=" << fp << ")\n";
  return 0;
}

//----------------------------------------------------------------------
int FSystemTest::putchar (int c)
{
  std::cerr << "Call: putchar (" << c << ")\n";
  characters.push_back(c);
  return 1;
}

//----------------------------------------------------------------------
int FSystemTest::tputs (const char* str, int affcnt, int (*putc)(int))
{
  return ::tputs (str, affcnt, putc);
}

//----------------------------------------------------------------------
uid_t FSystemTest::getuid()
{
  return 0;
}

//----------------------------------------------------------------------
std::string& FSystemTest::getCharacters()
{
  return characters;
}

//----------------------------------------------------------------------
int& FSystemTest::getCursorType()
{
  return cursor_type;
}




}  // namespace test


//----------------------------------------------------------------------
// class ftermfreebsdTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class ftermfreebsdTest : public CPPUNIT_NS::TestFixture, test::ConEmu
{
  public:
    ftermfreebsdTest();

  protected:
    void classNameTest();
    void netbsdConsoleTest();
    void freebsdConsoleTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (ftermfreebsdTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (freebsdConsoleTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
ftermfreebsdTest::ftermfreebsdTest()
{

}

//----------------------------------------------------------------------
void ftermfreebsdTest::classNameTest()
{
  const finalcut::FTermFreeBSD p;
  const char* const classname = p.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname, "FTermFreeBSD") == 0 );
}


//----------------------------------------------------------------------
void ftermfreebsdTest::freebsdConsoleTest()
{
  setenv ("TERM", "xterm", 1);
  setenv ("COLUMNS", "80", 1);
  setenv ("LINES", "25", 1);

  finalcut::FTermData* data;
  finalcut::FSystem* fsys = new test::FSystemTest();
  finalcut::FTermDetection* term_detection;
  finalcut::FTerm::setFSystem(fsys);
  std::cout << "\n";
  data = finalcut::FTerm::getFTermData();

  auto& encoding_list = data->getEncodingList();
  encoding_list["UTF-8"] = finalcut::fc::UTF8;
  encoding_list["UTF8"]  = finalcut::fc::UTF8;
  encoding_list["VT100"] = finalcut::fc::VT100;
  encoding_list["PC"]    = finalcut::fc::PC;
  encoding_list["ASCII"] = finalcut::fc::ASCII;

  data->setTermEncoding(finalcut::fc::VT100);
  data->setBaudrate(9600);
  data->setTermType("xterm");
  data->setTermFileName("/dev/ttyv0");
  data->setTTYFileDescriptor(0);
  data->supportShadowCharacter (false);
  data->supportHalfBlockCharacter (false);
  data->supportCursorOptimisation (true);
  data->setCursorHidden (true);
  data->useAlternateScreen (false);
  data->setASCIIConsole (true);
  data->setVT100Console (false);
  data->setUTF8Console (false);
  data->setUTF8 (false);
  data->setNewFont (false);
  data->setVGAFont (false);
  data->setMonochron (false);
  data->setTermResized (false);

  term_detection = finalcut::FTerm::getFTermDetection();
  term_detection->setTerminalDetection(true);
  pid_t pid = forkConEmu();

  if ( isConEmuChildProcess(pid) )
  {
    finalcut::FTermFreeBSD freebsd;

    setenv ("TERM", "xterm", 1);
    setenv ("COLUMNS", "80", 1);
    setenv ("LINES", "25", 1);
    unsetenv("TERMCAP");
    unsetenv("COLORTERM");
    unsetenv("COLORFGBG");
    unsetenv("VTE_VERSION");
    unsetenv("XTERM_VERSION");
    unsetenv("ROXTERM_ID");
    unsetenv("KONSOLE_DBUS_SESSION");
    unsetenv("KONSOLE_DCOP");
    unsetenv("TMUX");

    finalcut::FTerm::detectTermSize();
    freebsd.enableMetaSendsEscape();
    freebsd.enableChangeCursorStyle();
    freebsd.init();
    term_detection->detect();

#if DEBUG
    const finalcut::FString& sec_da = \
        finalcut::FTerm::getFTermDebugData().getSecDAString();
    CPPUNIT_ASSERT ( sec_da == "\033[>0;10;0c" );
#endif

    CPPUNIT_ASSERT ( isatty(0) == 1 );
    CPPUNIT_ASSERT ( term_detection->isFreeBSDTerm() );
    CPPUNIT_ASSERT ( data->getTermGeometry().getWidth() == 80 );
    CPPUNIT_ASSERT ( data->getTermGeometry().getHeight() == 25 );
    CPPUNIT_ASSERT ( ! data->hasShadowCharacter() );
    CPPUNIT_ASSERT ( ! data->hasHalfBlockCharacter() );

    test::FSystemTest* fsystest = static_cast<test::FSystemTest*>(fsys);
    freebsd.setCursorStyle (finalcut::fc::normal_cursor, false);
    CPPUNIT_ASSERT ( fsystest->getCursorType() == finalcut::fc::normal_cursor );

    freebsd.setCursorStyle (finalcut::fc::blink_cursor, false);
    CPPUNIT_ASSERT ( fsystest->getCursorType() == finalcut::fc::blink_cursor );

    freebsd.setCursorStyle (finalcut::fc::destructive_cursor, false);
    CPPUNIT_ASSERT ( fsystest->getCursorType() == finalcut::fc::destructive_cursor );

    std::string& characters = fsystest->getCharacters();
    characters.clear();
    freebsd.setBeep (20, 100);    // Hz < 21
    CPPUNIT_ASSERT ( characters.empty() );
    freebsd.setBeep (32767, 100); // Hz > 32766
    CPPUNIT_ASSERT ( characters.empty() );
    freebsd.setBeep (200, -1);    // ms < 0
    CPPUNIT_ASSERT ( characters.empty() );
    freebsd.setBeep (200, 2000);  // ms > 1999
    CPPUNIT_ASSERT ( characters.empty() );
    freebsd.setBeep (200, 100);   // 200 Hz - 100 ms

    CPPUNIT_ASSERT ( characters == CSI "=5965;10B" );
    characters.clear();
    freebsd.resetBeep();
    CPPUNIT_ASSERT ( characters == CSI "=800;5B" );
    characters.clear();

    freebsd.finish();

    closeConEmuStdStreams();
    exit(EXIT_SUCCESS);
  }
  else  // Parent
  {
    // Start the terminal emulation
    startConEmuTerminal (ConEmu::freebsd_con);

    if ( waitpid(pid, 0, WUNTRACED) != pid )
      std::cerr << "waitpid error" << std::endl;
  }
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (ftermfreebsdTest);

// The general unit test main part
#include <main-test.inc>
