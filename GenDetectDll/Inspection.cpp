#include "stdafx.h"
#include "Inspection.h"
#include "ColorDetect.cpp"

BOOL HObjectToBitmap(Hobject ho_Image, CBitmap& bitmap)
{
	try
	{
		Hobject  ho_ImageConverted;
		convert_image_type(ho_Image, &ho_ImageConverted, "byte");

		HTuple  hv_Channels;
		count_channels(ho_ImageConverted, &hv_Channels);

		HTuple  hv_Type, hv_Width, hv_Height;
		HTuple  hv_Pointer, hv_PointerRed, hv_PointerGreen, hv_PointerBlue;

		switch (static_cast<Hlong>(hv_Channels[0]))
		{
		case 1L:
			get_image_pointer1(ho_ImageConverted, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);
			hv_PointerBlue = hv_Pointer;
			hv_PointerRed = hv_Pointer;
			hv_PointerGreen = hv_Pointer;
			break;

		case 3L:
			get_image_pointer3(ho_ImageConverted, &hv_PointerRed, &hv_PointerGreen, &hv_PointerBlue, &hv_Type, &hv_Width, &hv_Height);
		}

		long lWidth(static_cast<Hlong>(hv_Width[0]));
		long lHeight(static_cast<Hlong>(hv_Height[0]));
		LPBYTE pRed(reinterpret_cast<LPBYTE>(hv_PointerRed[0].L()));
		LPBYTE pGreen(reinterpret_cast<LPBYTE>(hv_PointerGreen[0].L()));
		LPBYTE pBlue(reinterpret_cast<LPBYTE>(hv_PointerBlue[0].L()));

		long nBitcount(32L);
		long lWidthBytes(((lWidth * nBitcount + 31L) >> 5L) << 2L);
		long lBufferSize(lWidthBytes * lHeight);
		LPBYTE pBitmapData(reinterpret_cast<LPBYTE>(VirtualAlloc(NULL, lBufferSize, MEM_COMMIT, PAGE_READWRITE)));

		if (!pBitmapData)
		{
			return FALSE;
		}

		try
		{
			ZeroMemory(pBitmapData, lBufferSize);

			for (long i(0L); i < lHeight; i++)
			{
				long lOffset1(i * lWidthBytes);
				long lOffset2(i * lWidth);

				for (long j(0L); j < lWidth; j++)
				{
					pBitmapData[lOffset1++] = pBlue[lOffset2 + j];
					pBitmapData[lOffset1++] = pGreen[lOffset2 + j];
					pBitmapData[lOffset1++] = pRed[lOffset2 + j];
					pBitmapData[lOffset1++] = 0;
				}
			}
		}
		catch (...)
		{
			VirtualFree(pBitmapData, 0, MEM_RELEASE);
			throw;
		}

		bitmap.DeleteObject();
		bitmap.CreateBitmap(lWidth, lHeight, 1, nBitcount, pBitmapData);
		VirtualFree(pBitmapData, 0, MEM_RELEASE);
		return TRUE;
	}
	catch (...)
	{
	}
	return FALSE;
}

BOOL HObjectToCImage(Hobject ho_Image, CImage& image)
{
	CBitmap bitmap;

	if (!HObjectToBitmap(ho_Image, bitmap))
	{
		return FALSE;
	}

	image.Destroy();
	image.Attach(bitmap.operator HBITMAP());
	bitmap.Detach();
	return TRUE;
}

BOOL HObjectToHBitmap(Hobject ho_Image, HBITMAP& hBitmap)
{
	CImage image;
	if (!HObjectToCImage(ho_Image, image))
	{
		hBitmap = NULL;
		return FALSE;
	}

	hBitmap = static_cast<HBITMAP>(image.operator HBITMAP());
	image.Detach();
	return TRUE;
}

HTuple AcqHandle = 0;

extern "C"
{
	BOOL WINAPI HasConnectedToCamera()
	{
		try
		{
			return AcqHandle != NULL;
		}
		catch (...)
		{
		}
		return FALSE;
	}

	BOOL WINAPI ConnectToCamera()
	{
		try
		{
			if (!HasConnectedToCamera())
			{
				connect_to_camera(&AcqHandle);
				return AcqHandle != NULL;
			}
			return TRUE;
		}
		catch (...)
		{
		}

		AcqHandle = NULL;
		return FALSE;
	}

	BOOL WINAPI DisconnectFromCamera()
	{
		try
		{
			if (HasConnectedToCamera())
			{
				disconnect_from_camera(AcqHandle);
				AcqHandle = NULL;
			}
			return TRUE;
		}
		catch (...)
		{
		}

		AcqHandle = NULL;
		return FALSE;
	}

	BOOL WINAPI GrabImage(CImage& lpImage)
	{
		if (!HasConnectedToCamera() && !ConnectToCamera())
		{
			return FALSE;
		}

		Hobject ho_image;
		try
		{
			grab_image_from_camera(&ho_image, AcqHandle);
			return HObjectToCImage(ho_image, lpImage);
		}
		catch (...)
		{
			if (DisconnectFromCamera() && ConnectToCamera())
			{
				try
				{
					grab_image_from_camera(&ho_image, AcqHandle);
					return HObjectToCImage(ho_image, lpImage);
				}
				catch (...)
				{
				}
			}
		}

		return FALSE;
	}

	BOOL WINAPI GrabHImage(HBITMAP& hImage)
	{
		if (!HasConnectedToCamera() && !ConnectToCamera())
		{
			return FALSE;
		}

		Hobject ho_image;
		try
		{
			grab_image_from_camera(&ho_image, AcqHandle);
			return HObjectToHBitmap(ho_image, hImage);
		}
		catch (...)
		{
			if (DisconnectFromCamera() && ConnectToCamera())
			{
				try
				{
					grab_image_from_camera(&ho_image, AcqHandle);
					return HObjectToHBitmap(ho_image, hImage);
				}
				catch (...)
				{
				}
			}
		}

		return FALSE;
	}

	BOOL WINAPI GrabImageFromFileA(LPCSTR lpszFileName, CImage& lpImage)
	{
		CStringA txFileName(lpszFileName);
		txFileName.Replace('\\', '/');

		Hobject ho_image;
		try
		{
			grab_image_from_file(&ho_image, HTuple(txFileName.GetString()));
			return HObjectToCImage(ho_image, lpImage);
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI GrabHImageFromFileA(LPCSTR lpszFileName, HBITMAP& hImage)
	{
		CStringA txFileName(lpszFileName);
		txFileName.Replace('\\', '/');

		Hobject ho_image;
		try
		{
			grab_image_from_file(&ho_image, HTuple(txFileName.GetString()));
			return HObjectToHBitmap(ho_image, hImage);
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI GrabImageFromFileW(LPCWSTR lpszFileName, CImage& lpImage)
	{
		CStringA txFileName;
		txFileName.Format("%S", lpszFileName);

		return GrabImageFromFileA(txFileName, lpImage);
	}

	BOOL WINAPI GrabHImageFromFileW(LPCWSTR lpszFileName, HBITMAP& hImage)
	{
		CStringA txFileName;
		txFileName.Format("%S", lpszFileName);

		return GrabHImageFromFileA(txFileName, hImage);
	}

	BOOL WINAPI DoInspect1(CImage& lpImage, CImage& lpReport, LONG& result)
	{
		if (!HasConnectedToCamera() && !ConnectToCamera())
		{
			return FALSE;
		}

		Hobject ho_image;
		BOOL bResult(FALSE);

		try
		{
			grab_image_from_camera(&ho_image, AcqHandle);
			bResult = TRUE;
		}
		catch (...)
		{
			if (DisconnectFromCamera() && ConnectToCamera())
			{
				try
				{
					grab_image_from_camera(&ho_image, AcqHandle);
					bResult = TRUE;
				}
				catch (...)
				{
				}
			}
		}

		if (!bResult)
			return FALSE;

		LONG lX(GetSystemMetrics(SM_CXSCREEN));
		LONG lY(GetSystemMetrics(SM_CYSCREEN));


		try
		{
			Hobject ho_report;
			HTuple hv_result;
			CStringA _WorkDirectory = "";
			do_inspect1(ho_image, &ho_report, _WorkDirectory.GetString(), lX, lY, &hv_result);

			result = static_cast<Hlong>(hv_result[0]);

			if (!HObjectToCImage(ho_image, lpImage))
			{
				lpImage.Destroy();
				return FALSE;
			}

			if (!HObjectToCImage(ho_report, lpReport))
			{
				lpImage.Destroy();
				lpReport.Destroy();
				return FALSE;
			}

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI DoHInspect1(HBITMAP& hImage, HBITMAP& hReport, LONG& result)
	{
		if (!HasConnectedToCamera() && !ConnectToCamera())
		{
			return FALSE;
		}

		Hobject ho_image;
		BOOL bResult(FALSE);

		try
		{
			grab_image_from_camera(&ho_image, AcqHandle);
			bResult = TRUE;
		}
		catch (...)
		{
			if (DisconnectFromCamera() && ConnectToCamera())
			{
				try
				{
					grab_image_from_camera(&ho_image, AcqHandle);
					bResult = TRUE;
				}
				catch (...)
				{
				}
			}
		}

		if (!bResult)
			return FALSE;

		LONG lX(GetSystemMetrics(SM_CXSCREEN));
		LONG lY(GetSystemMetrics(SM_CYSCREEN));


		try
		{
			Hobject ho_report;
			HTuple hv_result;
			CStringA _WorkDirectory = "";
			do_inspect1(ho_image, &ho_report, _WorkDirectory.GetString(), lX, lY, &hv_result);

			result = static_cast<Hlong>(hv_result[0]);

			if (!HObjectToHBitmap(ho_image, hImage))
			{
				hImage = NULL;
				return FALSE;
			}

			if (!HObjectToHBitmap(ho_report, hReport))
			{
				hImage = NULL;
				hReport = NULL;
				return FALSE;
			}

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI DoInspect1FromFileA(LPCSTR lpszFileName, CImage& lpImage, 
	                                CImage& lpReport, LONG& result)
	{
		try
		{
			CStringA txFileName(lpszFileName);
			txFileName.Replace('\\', '/');

			Hobject ho_image;
			grab_image_from_file(&ho_image, HTuple(txFileName.GetString()));

			LONG lX(GetSystemMetrics(SM_CXSCREEN));
			LONG lY(GetSystemMetrics(SM_CYSCREEN));

			Hobject ho_report;
			HTuple hv_result;
			CStringA _WorkDirectory = "";
			do_inspect1(ho_image, &ho_report, _WorkDirectory.GetString(), lX, lY, &hv_result);
			result = static_cast<Hlong>(hv_result[0]);

			if (!HObjectToCImage(ho_image, lpImage))
			{
				lpImage.Destroy();
				return FALSE;
			}

			if (!HObjectToCImage(ho_report, lpReport))
			{
				lpImage.Destroy();
				lpReport.Destroy();
				return FALSE;
			}

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI DoHInspect1FromFileA(LPCSTR lpszFileName, HBITMAP& hImage, 
	                                 HBITMAP& hReport, LONG& result)
	{
		try
		{
			CStringA txFileName(lpszFileName);
			txFileName.Replace('\\', '/');

			Hobject ho_image;
			grab_image_from_file(&ho_image, HTuple(txFileName.GetString()));

			LONG lX(GetSystemMetrics(SM_CXSCREEN));
			LONG lY(GetSystemMetrics(SM_CYSCREEN));

			Hobject ho_report;
			HTuple hv_result;
			CStringA _WorkDirectory = "";
			do_inspect1(ho_image, &ho_report, _WorkDirectory.GetString(), lX, lY, &hv_result);
			result = static_cast<Hlong>(hv_result[0]);

			if (!HObjectToHBitmap(ho_image, hImage))
			{
				hImage = NULL;
				hReport = NULL;
				return FALSE;
			}

			if (!HObjectToHBitmap(ho_report, hReport))
			{
				hImage = NULL;
				hReport = NULL;
				return FALSE;
			}

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI DoInspect1FromFileW(LPCWSTR lpszFileName, CImage& lpImage, 
	                                CImage& lpReport, LONG& result)
	{
		CStringA txFileName;
		txFileName.Format("%S", lpszFileName);

		return DoInspect1FromFileA(txFileName, lpImage, lpReport, result);
	}

	BOOL WINAPI DoHInspect1FromFileW(LPCWSTR lpszFileName, HBITMAP& hImage, 
	                                 HBITMAP& hReport, LONG& result)
	{
		CStringA txFileName;
		txFileName.Format("%S", lpszFileName);

		return DoHInspect1FromFileA(txFileName, hImage, hReport, result);
	}

	BOOL WINAPI DoInspect2(CImage& lpImage, CImage& lpReport, LONG& result)
	{
		if (!HasConnectedToCamera() && !ConnectToCamera())
		{
			return FALSE;
		}

		Hobject ho_image;
		BOOL bResult(FALSE);

		try
		{
			grab_image_from_camera(&ho_image, AcqHandle);
			bResult = TRUE;
		}
		catch (...)
		{
			if (DisconnectFromCamera() && ConnectToCamera())
			{
				try
				{
					grab_image_from_camera(&ho_image, AcqHandle);
					bResult = TRUE;
				}
				catch (...)
				{
				}
			}
		}

		if (!bResult)
			return FALSE;

		LONG lX(GetSystemMetrics(SM_CXSCREEN));
		LONG lY(GetSystemMetrics(SM_CYSCREEN));


		try
		{
			Hobject ho_report;
			HTuple hv_result;
			CStringA _WorkDirectory = "";
			do_inspect2(ho_image, &ho_report, _WorkDirectory.GetString(), lX, lY, &hv_result);

			result = static_cast<Hlong>(hv_result[0]);

			if (!HObjectToCImage(ho_image, lpImage))
			{
				lpImage.Destroy();
				return FALSE;
			}

			if (!HObjectToCImage(ho_report, lpReport))
			{
				lpImage.Destroy();
				lpReport.Destroy();
				return FALSE;
			}

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI DoHInspect2(HBITMAP& hImage, HBITMAP& hReport, LONG& result)
	{
		if (!HasConnectedToCamera() && !ConnectToCamera())
		{
			return FALSE;
		}

		Hobject ho_image;
		BOOL bResult(FALSE);

		try
		{
			grab_image_from_camera(&ho_image, AcqHandle);
			bResult = TRUE;
		}
		catch (...)
		{
			if (DisconnectFromCamera() && ConnectToCamera())
			{
				try
				{
					grab_image_from_camera(&ho_image, AcqHandle);
					bResult = TRUE;
				}
				catch (...)
				{
				}
			}
		}

		if (!bResult)
			return FALSE;

		LONG lX(GetSystemMetrics(SM_CXSCREEN));
		LONG lY(GetSystemMetrics(SM_CYSCREEN));


		try
		{
			Hobject ho_report;
			HTuple hv_result;
			CStringA _WorkDirectory = "";
			do_inspect2(ho_image, &ho_report, _WorkDirectory.GetString(), lX, lY, &hv_result);

			result = static_cast<Hlong>(hv_result[0]);

			if (!HObjectToHBitmap(ho_image, hImage))
			{
				hImage = NULL;
				return FALSE;
			}

			if (!HObjectToHBitmap(ho_report, hReport))
			{
				hImage = NULL;
				hReport = NULL;
				return FALSE;
			}

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI DoInspect2FromFileA(LPCSTR lpszFileName, CImage& lpImage, 
	                                CImage& lpReport, LONG& result)
	{
		try
		{
			CStringA txFileName(lpszFileName);
			txFileName.Replace('\\', '/');

			Hobject ho_image;
			grab_image_from_file(&ho_image, HTuple(txFileName.GetString()));

			LONG lX(GetSystemMetrics(SM_CXSCREEN));
			LONG lY(GetSystemMetrics(SM_CYSCREEN));

			Hobject ho_report;
			HTuple hv_result;
			CStringA _WorkDirectory = "";
			do_inspect2(ho_image, &ho_report, _WorkDirectory.GetString(), lX, lY, &hv_result);
			result = static_cast<Hlong>(hv_result[0]);

			if (!HObjectToCImage(ho_image, lpImage))
			{
				lpImage.Destroy();
				return FALSE;
			}

			if (!HObjectToCImage(ho_report, lpReport))
			{
				lpImage.Destroy();
				lpReport.Destroy();
				return FALSE;
			}

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI DoHInspect2FromFileA(LPCSTR lpszFileName, HBITMAP& hImage, 
	                                 HBITMAP& hReport, LONG& result)
	{
		try
		{
			CStringA txFileName(lpszFileName);
			txFileName.Replace('\\', '/');

			Hobject ho_image;
			grab_image_from_file(&ho_image, HTuple(txFileName.GetString()));

			LONG lX(GetSystemMetrics(SM_CXSCREEN));
			LONG lY(GetSystemMetrics(SM_CYSCREEN));

			Hobject ho_report;
			HTuple hv_result;
			CStringA _WorkDirectory = "";
			do_inspect2(ho_image, &ho_report, _WorkDirectory.GetString(), lX, lY, &hv_result);
			result = static_cast<Hlong>(hv_result[0]);

			if (!HObjectToHBitmap(ho_image, hImage))
			{
				hImage = NULL;
				hReport = NULL;
				return FALSE;
			}

			if (!HObjectToHBitmap(ho_report, hReport))
			{
				hImage = NULL;
				hReport = NULL;
				return FALSE;
			}

			return TRUE;
		}
		catch (...)
		{
		}

		return FALSE;
	}

	BOOL WINAPI DoInspect2FromFileW(LPCWSTR lpszFileName, CImage& lpImage, 
	                                CImage& lpReport, LONG& result)
	{
		CStringA txFileName;
		txFileName.Format("%S", lpszFileName);

		return DoInspect2FromFileA(txFileName, lpImage, lpReport, result);
	}

	BOOL WINAPI DoHInspect2FromFileW(LPCWSTR lpszFileName, HBITMAP& hImage, 
	                                 HBITMAP& hReport, LONG& result)
	{
		CStringA txFileName;
		txFileName.Format("%S", lpszFileName);

		return DoHInspect2FromFileA(txFileName, hImage, hReport, result);
	}
}
