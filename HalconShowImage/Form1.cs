using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HalconShowImage
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent(     );
        }

        private void btnShow_Click(object sender, EventArgs e)
        {
            string imgPath = @"D:\Work\LED\blue.bmp";
            Bitmap src, report;
            bool isPass = HImageClass.DoInspectFromFile(false, out src,
                out report, imgPath);
            pbImage.Image = report;
        }
    }
}
