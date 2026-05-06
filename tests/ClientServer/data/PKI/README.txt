This directory contains two different PKIs used for unit tests on 'SOPC_PKIProvider_CreateFromStore' function.

'only_bad_certs' contains two invalid certificates (one PEM and one DER). This PKI should be rejected by the unit test.

'valide_certs_plus_bad_certs' is based on 'S2OPC_Demo_PKI' with two invalid certificates added in the trusted subdirectory. This PKI should be accepted if the 'S2OPC_START_APP_PKI_WITH_ONE_VALID_CERT' compilation flag is enabled. Otherwise, it should be rejected.

______________________________________________________________________________________________________________________
'bad_cert.pem' was created with the following command :

cat > bad_cert.pem <<'EOF'
-----BEGIN CERTIFICATE-----
this is definitely not a valid PEM encoded certificate
-----END CERTIFICATE-----
EOF

______________________________________________________________________________________________________________________
'bad_cert2.der' was created with the following command :

echo "this is definitely not a valid DER encoded certificate" > bad_cert2.der
