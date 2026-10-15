"""Create a dedicated TLS certificate for development authentication endpoints."""

import argparse
import datetime
import ipaddress
from pathlib import Path

from cryptography import x509
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.x509.oid import NameOID


def create(directory, hosts):
  directory.mkdir(parents=True, exist_ok=True)
  certificate_path = directory / "authentication_certificate.pem"
  key_path = directory / "authentication_private_key.pem"
  if certificate_path.exists() or key_path.exists():
    raise ValueError("Certificate output already exists; choose another directory")
  names = []
  for host in hosts:
    try:
      names.append(x509.IPAddress(ipaddress.ip_address(host)))
    except ValueError:
      names.append(x509.DNSName(host.encode("idna").decode("ascii")))
  key = rsa.generate_private_key(public_exponent=65537, key_size=3072)
  name = x509.Name([x509.NameAttribute(NameOID.COMMON_NAME, hosts[0])])
  now = datetime.datetime.now(datetime.timezone.utc)
  certificate = (x509.CertificateBuilder().subject_name(name).issuer_name(name)
         .public_key(key.public_key()).serial_number(x509.random_serial_number())
         .not_valid_before(now - datetime.timedelta(minutes=1))
         .not_valid_after(now + datetime.timedelta(days=30))
         .add_extension(x509.SubjectAlternativeName(names), critical=False)
         .add_extension(x509.BasicConstraints(ca=False, path_length=None), critical=True)
         .add_extension(x509.ExtendedKeyUsage([x509.oid.ExtendedKeyUsageOID.SERVER_AUTH]), critical=False)
         .sign(key, hashes.SHA256()))
  with key_path.open("xb") as output:
    output.write(key.private_bytes(serialization.Encoding.PEM, serialization.PrivateFormat.PKCS8,
                   serialization.NoEncryption()))
  with certificate_path.open("xb") as output:
    output.write(certificate.public_bytes(serialization.Encoding.PEM))
  return certificate_path


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("--directory", type=Path, required=True)
  parser.add_argument("--host", action="append")
  arguments = parser.parse_args()
  print(create(arguments.directory, arguments.host or ["localhost", "127.0.0.1", "::1"]))
