using System;
using System.Drawing;
using System.Runtime.InteropServices;

namespace HalconShowImage
{
    public class HImageClass
    {
        private const string DllPath = @"C:\Work\GenDetectDll\Debug\GenDetectDll.dll";

        #region DllImport

        [DllImport(DllPath, EntryPoint = "GrabHImage",
            CallingConvention = CallingConvention.Winapi)]
        private static extern bool GrabHImage(
            ref IntPtr pImage);

        [DllImport(DllPath, EntryPoint = "GrabHImageFromFileA",
            CallingConvention = CallingConvention.Winapi)]
        private static extern bool GrabHImageFromFileA(
            string path,
            ref IntPtr pImage);

        [DllImport(DllPath, EntryPoint = "DoHInspect1",
            CallingConvention = CallingConvention.Winapi)]
        private static extern bool DoHInspect1(
            ref IntPtr pImage,
            ref IntPtr pReport,
            ref uint result);

        [DllImport(DllPath, EntryPoint = "DoHInspect1FromFileA",
            CallingConvention = CallingConvention.Winapi)]
        private static extern bool DoHInspect1FromFileA(
            string path,
            ref IntPtr pImage,
            ref IntPtr pReport,
            ref uint result);

        [DllImport(DllPath, EntryPoint = "DoHInspect2",
            CallingConvention = CallingConvention.Winapi)]
        private static extern bool DoHInspect2(
            ref IntPtr pImage,
            ref IntPtr pReport,
            ref uint result);

        [DllImport(DllPath, EntryPoint = "DoHInspect2FromFileA",
            CallingConvention = CallingConvention.Winapi)]
        private static extern bool DoHInspect2FromFileA(
            string path,
            ref IntPtr pImage,
            ref IntPtr pReport,
            ref uint result);

        #endregion


        /// <summary>
        /// 从文件获取检测结果
        /// </summary>
        /// <param name="isOne">是否使用doInspect1</param>
        /// <param name="bmpOri">原始图像</param>
        /// <param name="bmpReport">结果图像</param>
        /// <param name="path">图片路径</param>
        /// <returns>OK/NG</returns>
        public static bool DoInspectFromFile(bool isOne, out Bitmap bmpOri,
            out Bitmap bmpReport, string path)
        {
            IntPtr pOri = new IntPtr();
            IntPtr pReport = new IntPtr();
            uint result = 0;
            if (isOne)
            {
                DoHInspect1FromFileA(path, ref pOri, ref pReport, ref result);
                bmpOri = Image.FromHbitmap(pOri);
                bmpReport = Image.FromHbitmap(pReport);
                return result != 0;
            }
            else
            {
                DoHInspect2FromFileA(path, ref pOri, ref pReport, ref result);
                bmpOri = Image.FromHbitmap(pOri);
                bmpReport = Image.FromHbitmap(pReport);
                return result != 0;
            }
        }

        /// <summary>
        /// 从相机抓图并获取检测结果
        /// </summary>
        /// <param name="isOne"></param>
        /// <param name="bmpOri"></param>
        /// <param name="bmpReport"></param>
        /// <returns></returns>
        public static bool DoInspect(bool isOne, out Bitmap bmpOri,
            out Bitmap bmpReport)
        {
            IntPtr pOri = new IntPtr();
            IntPtr pReport = new IntPtr();
            uint result = 0;
            if (isOne)
            {
                DoHInspect1(ref pOri, ref pReport, ref result);
                bmpOri = Image.FromHbitmap(pOri);
                bmpReport = Image.FromHbitmap(pReport);
                return result != 0;
            }
            else
            {
                DoHInspect2(ref pOri, ref pReport, ref result);
                bmpOri = Image.FromHbitmap(pOri);
                bmpReport = Image.FromHbitmap(pReport);
                return result != 0;
            }
        }

        /// <summary>
        /// 从文件中获取图像
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public static Bitmap GrabImageFromFile(string path)
        {
            IntPtr pImage = new IntPtr();
            return GrabHImageFromFileA(path, ref pImage)
                ? Image.FromHbitmap(pImage)
                : null;
        }

        /// <summary>
        /// 从相机中获取图像
        /// </summary>
        /// <returns></returns>
        public static Bitmap GrabImageFromCamera()
        {
            IntPtr pImage = new IntPtr();
            return GrabHImage(ref pImage)
                ? Image.FromHbitmap(pImage)
                : null;
        }


    }
}