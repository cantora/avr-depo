#avr-depo
AVR Deterministic Embedded Password Oracle facilitates password generation
on an embedded AVR platform (e.g. Arduino) using a master password provided
by the user. Because it is embedded in hardware, AVR-depo provides trusted
password input unlike online/PC equivalents which cannot avoid the risk of
master password comprimise via trojan.

##overview
avr-depo helps mitigate ["password hell"](https://en.wikipedia.org/wiki/Password_fatigue) 
by generating derived passwords using a master password combined with 
an *alias* and a *name*. Usually *name* will be the name of a particular 
service or website. The *alias* can be used to maintain different 
accounts/passwords for the same service/website using different identities. 
For example, one could use a 'work' alias for all work related passwords 
(*alias* can be left blank, which could be considered the 'default' identity). 
Deterministic algorithms are used for password generation such that given 
the same master password, *alias* and *name*, avr-depo will generate the same 
output password. This means that the passwords are not stored anywhere, 
but remain accessible to only the person who knows the master password.

##target platforms
Only the [Arduino](http://arduino.cc/) platform is currently supported.

##project status
The arduino firmware is currently only a proof of concept. Some
planned features are missing and the code has not been audited for 
vulnerabilities.

I'm currently working on creating a PCB prototype from my breadboarded 
prototype.

##technical details
 * master key generation: [PBKDF2](https://en.wikipedia.org/wiki/PBKDF2) is
   used to transform the master password into key material for use by the
   password generation algorithms. The salt and PBKDF2 rounds parameters
   are configurable (see the configuration section). The PBKDF2 source
   code (`src/crypto_pbkdf2.c`) was ported from the openssl implementation.

 * password generation: the algorithms all consume the key bytes produced
   by PBKDF2 in order to make pseudo-random choices which lead to a 
   password satisfying the parameters of the chosen algorithm. Different
   algorithms will use different amounts of key data. The implementations
   can be found in `src/schema.c`.

##technical details on password schemas
Presently only two password generation schemas have been implemented, but
many more are planned.

 * "hex": this schema converts generated key bytes to their hex
   representations. One byte of master key is consumed per two 
   characters of output.
 * "pw0": consumes generated key bytes to make random choices leading
   to the generation of a password with at least one upper case letter,
   at least one lower case letter, at least one symbol and at least one
   number. This schema consumes at least four bytes of master key per
   character of output. Additionally, a number of bytes are consumed to
   decide how many of each character class will be represented and to
   shuffle the different classes together.

##configuration
This code is not ready for the end user, but if you want to try it out anyway
you should configure the PBKDF2 settings in `src/avr-depo-config.h`. Don't
consider the settings there to be 'secure', they will be set to values which
I find convenient/useful for my development of avr-depo.

##building
build instructions coming soon...

##license
[GPLv3](http://www.gnu.org/licenses/gpl-3.0.html). See LICENSE or the 
given URL for details.  
