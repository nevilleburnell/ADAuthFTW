using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.DirectoryServices;

namespace adauthftw
{
    public class adauthftwClass
    {

        public bool Authenticate(string path, string user, string pass)
        {
            try
            {
                var ad = new DirectoryEntry(path, user, pass);
                if (ad.Properties.Count > 0) return true;
            }
            catch (Exception ex)
            {
                return false;
            }
            return false;
        }

        public adauthftwClass()
        {

        }
    }
}
