/*
 * Pch.h is part of the NetPlay distribution (https://bckim.com)
 * Copyright (c) 2012 bckim
 */
#ifndef _PCH_H_
#define _PCH_H_

#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <cmath>
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <NetPlay.h>

#if defined(WXUSINGDLL)
#include <wx/wx.h>
#include <wx/cmdline.h>

#endif

#include "Typedef.h"
#include "Utility.h"
#include "Protocol.h"

#endif//_PCH_H_