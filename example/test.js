var ad = new (require('./adauthftw').adauthftw)();

ad.auth('LDAP://whatever.domain.com/DC=DOMAIN,DC=com',
  'testuser',
  'testpass', 
  function (err, result) {
    if (result) {
      console.log('Authentication successful');
    } else {
      console.log('Authentication failed.');
    }
});