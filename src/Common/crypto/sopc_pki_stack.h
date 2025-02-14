/*
 * Licensed to Systerel under one or more contributor license
 * agreements. See the NOTICE file distributed with this work
 * for additional information regarding copyright ownership.
 * Systerel licenses this file to you under the Apache
 * License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/** \file sopc_pki_stack.h
 *
 * \brief Defines the minimal PKI implementation provided by the stack
 *
 * The stack will not to provide a full-blown configurable PKI.
 * The stack provides only a minimal, always safe validating PKI.
 * The stack provides a thread-safe PKI, it is necessary
 * for OPC UA client use case (shared between services and secure channel layers)
 * and PKI trust list update feature (shared between S2OPC library layers and possibly application thread).
 *
 * See sopc_pki_stack_lib_itf.h for API.
 */

#ifndef SOPC_PKI_STACK_H_
#define SOPC_PKI_STACK_H_

#include "sopc_pki_stack_lib_itf.h"

/*
 * ------------------------------------------------------------------------------------------------
 * Create API
 * ------------------------------------------------------------------------------------------------
 */

/**
 * \brief Creates the PKIProvider from a directory where certificates are stored.
 *
 * The directory store shall be organized as follows:
 *
 * - \<Directory_store_name\>/trusted/certs (.DER or .PEM files)
 * - \<Directory_store_name\>/trusted/crl (.DER or .PEM files)
 * - \<Directory_store_name\>/issuers/certs (.DER or .PEM files)
 * - \<Directory_store_name\>/issuers/crl (.DER or .PEM files
 *
 * Optional updated trust list directory (for runtime update persistence) :
 *
 * - \<Directory_store_name\>/updatedTrustList/trusted/certs (.DER or .PEM files)
 * - \<Directory_store_name\>/updatedTrustList/trusted/crl (.DER or .PEM files)
 * - \<Directory_store_name\>/updatedTrustList/issuers/certs (.DER or .PEM files)
 * - \<Directory_store_name\>/updatedTrustList/issuers/crl (.DER or .PEM files)
 *
 * \note: file extension names are not checked and all files are considered valid certificates or CRL
 *        except for file names starting with a '.' in order to allow placeholders for empty directories.
 *
 * The function attempts to build the PKI from the updatedTrustList directory
 * and in case of error (missing, empty or malformed), it switches to the root trusted and issuers directories.
 *
 * Notions :
 * - CA is a root CA if it is self-signed.
 * - trusted/certs = trusted root CA + trusted link CA + trusted cert.
 * - trusted/crl = CRLs of the trusted root CA + trusted link CA.
 * - issuer/certs = untrusted root CA + untrusted link CA.
 * - issuer/crl = CRLs of the untrusted root CA + untrusted link CA.
 * - CAs from trusted/certs and issuers/certs allow to verify the signing chain of a cert which is included into
 *   trusted/certs.
 * - CAs from trusted/certs allow to verify the signing chain of a cert which is not included into trusted/certs.
 *
 * This function checks that :
 * - the number of certificates plus CRLs does not exceed \c SOPC_PKI_MAX_NB_CERT_AND_CRL .
 * - the certificate store is not empty.
 * - at least one trusted certificate is provided.
 * - each certificate from subfolder issuer/certs is CA.
 * - each CA has exactly one Certificate Revocation List (CRL).
 *
 * \note Content of the PKI is NULL when return value is not SOPC_STATUS_OK.
 *
 * \param directoryStorePath The directory path where certificates are stored.
 * \param[out] ppPKI A valid pointer to the newly created PKIProvider. You should free such provider with
 *                   ::SOPC_PKIProvider_Free().
 *
 * \return  SOPC_STATUS_OK when successful, SOPC_STATUS_INVALID_PARAMETERS when parameters are NULL,
 *          and SOPC_STATUS_NOK when there was an error.
 */
SOPC_ReturnStatus SOPC_PKIProvider_CreateFromStore(const char* directoryStorePath, SOPC_PKIProvider** ppPKI);

/**
 * \brief Create the PKIProvider from list representation.
 *
 * Notions :
 * - CA is a root CA if it is self-signed.
 * - \p pTrustedCerts = trusted root CA + trusted link CA + trusted cert.
 * - \p pTrustedCrl = CRLs of the trusted root CA + trusted link CA.
 * - \p pIssuerCerts = untrusted root CA + untrusted link CA.
 * - \p pIssuerCrl = CRLs of the untrusted root CA + untrusted link CA.
 * - CAs from trusted/certs and issuers/certs allow to verify the signing chain of a cert which is included into
 *   trusted/certs.
 * - CAs from trusted/certs allow to verify the signing chain of a cert which is not included into trusted/certs.
 *
 * This function checks that :
 * - the number of certificates plus CRLs does not exceed \c SOPC_PKI_MAX_NB_CERT_AND_CRL .
 * - at least one cert from \p pTrustedCerts is provided.
 * - each certificate from \p pIssuerCerts is CA.
 * - each CA has exactly one Certificate Revocation List (CRL).
 *
 * \param pTrustedCerts A valid pointer to the trusted certificate list.
 * \param pTrustedCrl A valid pointer to the trusted CRL list.
 * \param pIssuerCerts A valid pointer to the issuer certificate list. NULL if not used.
 * \param pIssuerCrl A valid pointer to the issuer CRL list. NULL if not used.
 * \param[out] ppPKI A valid pointer to the newly created PKIProvider. You should free such provider with
 *                   ::SOPC_PKIProvider_Free().
 *
 * \return  SOPC_STATUS_OK when successful, SOPC_STATUS_INVALID_PARAMETERS when parameters are NULL,
 *          and SOPC_STATUS_NOK when there was an error.
 */
SOPC_ReturnStatus SOPC_PKIProvider_CreateFromList(SOPC_CertificateList* pTrustedCerts,
                                                  SOPC_CRLList* pTrustedCrl,
                                                  SOPC_CertificateList* pIssuerCerts,
                                                  SOPC_CRLList* pIssuerCrl,
                                                  SOPC_PKIProvider** ppPKI);

/*
 * ------------------------------------------------------------------------------------------------
 * LeafProfile API
 * ------------------------------------------------------------------------------------------------
 */

/**
 * \brief Create a leaf certificate profile from security policy to check certificate properties.
 *
 *        KeyUsage, extendedKeyUsage, URI and HostName of subjectAltName are not configured here then
 *        these properties have to be defined manually or though specific functions eg
 *        ::SOPC_PKIProvider_LeafProfileSetUsageFromType , ::SOPC_PKIProvider_LeafProfileSetURI
 *        and ::SOPC_PKIProvider_LeafProfileSetURL
 *
 * \param securityPolicyUri The URI describing the security policy. If NULL then an empty profile is created.
 * \param[out] ppProfile The newly created leaf profile. You should delete it with
 * ::SOPC_PKIProvider_DeleteLeafProfile .
 *
 * \note If the profile is empty ( \p securityPolicyUri is NULL) then the functions that use this profile will not run
 * any checks.
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_CreateLeafProfile(const char* securityPolicyUri, SOPC_PKI_LeafProfile** ppProfile);

/**
 * \brief Set the application URI to the leaf profile.
 *
 * \param pProfile A valid pointer to the leaf profile.
 * \param applicationUri The application URI to set in \p pProfile .
 *
 * \warning If the application URI is already defined in \p pProfile, you can not define it again.
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_LeafProfileSetURI(SOPC_PKI_LeafProfile* pProfile, const char* applicationUri);

/**
 * \brief Set the endpoint URL used for connection to the leaf profile.
 *
 * \param pProfile A valid pointer to the leaf profile.
 * \param url The endpoint URL used for connection to set in \p pProfile .
 *
 * \warning If the URL is already defined in \p pProfile , you can not define it again.
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_LeafProfileSetURL(SOPC_PKI_LeafProfile* pProfile, const char* url);

/**
 * \brief Set the keyUsage and extendedKeyUsage to the leaf profile from the PKI type.
 *
 *        For users : the keyUsage is expected to be filled with digitalSignature and the extendedKeyUsage is not
 *        checked.
 *        For clients : the keyUsage is expected to be filled with digitalSignature, nonRepudiation, keyEncipherment
 *        and dataEncipherment. The extendedKeyUsage is filled with serverAuth.
 *        For server : the keyUsage is expected to be filled with digitalSignature, nonRepudiation, keyEncipherment
 *        and dataEncipherment. The extendedKeyUsage is filled with clientAuth.
 *
 * \param pProfile A valid pointer to the leaf profile.
 * \param PKIType Defines the type of PKI (user, client or server)
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_LeafProfileSetUsageFromType(SOPC_PKI_LeafProfile* pProfile, SOPC_PKI_Type PKIType);

/** \brief Check leaf certificate properties
 *
 * \param pToValidate A valid pointer to the Certificate to validate.
 * \param pProfile A valid pointer to the leaf profile.
 * \param[out] error Pointer to store the OpcUa error code when certificate validation failed.
 *
 * \note \p error is only set if returned status is different from SOPC_STATUS_OK.
 *
 * \return SOPC_STATUS_OK when the certificate properties are successfully validated, and
 *         SOPC_STATUS_INVALID_PARAMETERS, SOPC_STATUS_INVALID_STATE or SOPC_STATUS_NOK.
 */
SOPC_ReturnStatus SOPC_PKIProvider_CheckLeafCertificate(const SOPC_CertificateList* pToValidate,
                                                        const SOPC_PKI_LeafProfile* pProfile,
                                                        uint32_t* error);

/**
 * \brief Delete a leaf profile.
 *
 * \param ppProfile The leaf profile.
 */
void SOPC_PKIProvider_DeleteLeafProfile(SOPC_PKI_LeafProfile** ppProfile);

/*
 * ------------------------------------------------------------------------------------------------
 * Profile API
 * ------------------------------------------------------------------------------------------------
 */

/**
 * \brief Create a PKI profile for a validation process.
 *        Backward interoperability is enabled.
 *        Leaf profile and chain profile are created according the security policy.
 *        KeyUsage, extendedKeyUsage, URI and HostName of subjectAltName are not configured here then
 *        these properties have to be defined manually or though specific functions eg
 *        ::SOPC_PKIProvider_ProfileSetUsageFromType , ::SOPC_PKIProvider_ProfileSetURI
 *        and ::SOPC_PKIProvider_ProfileSetURL
 *
 * \param securityPolicyUri The URI describing the security policy. Shall not be NULL.
 * \param[out] ppProfile The newly created profile. You should delete it with ::SOPC_PKIProvider_DeleteProfile .
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_CreateProfile(const char* securityPolicyUri, SOPC_PKI_Profile** ppProfile);

/**
 * \brief Create a minimal PKI profile for user validation process.
 *
 * \param ppProfile The newly created profile. You should delete it with ::SOPC_PKIProvider_DeleteProfile .
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_CreateMinimalUserProfile(SOPC_PKI_Profile** ppProfile);

/**
 * \brief Set the properties to the PKI profile from the PKI type.
 *
 *        For users : the backward interoperability is disabled and the leaf profile will not be applied during
 *        ::SOPC_PKIProvider_ValidateCertificate.
 *        For clients : the keyUsage is expected to be filled with digitalSignature,
 *        nonRepudiation, keyEncipherment and dataEncipherment. The extendedKeyUsage is filled with serverAuth. Finally
 *        the backward interoperability is enabled.
 *        For Server : the keyUsage is expected to be filled with digitalSignature, nonRepudiation, keyEncipherment
 *        and dataEncipherment. The extendedKeyUsage is filled with clientAuth. Finally the backward interoperability
 *        is enabled.
 *
 * \param pProfile A valid pointer to the PKI profile.
 * \param PKIType Defines the type of PKI (user, client or server)
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_ProfileSetUsageFromType(SOPC_PKI_Profile* pProfile, SOPC_PKI_Type PKIType);

/**
 * \brief Set the application URI to the PKI profile.
 *
 * \param pProfile A valid pointer to the PKI profile.
 * \param applicationUri The application URI to set in \p pProfile.
 *
 * \warning If the application URI is already defined in \p pProfile, you can not define it again.
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_ProfileSetURI(SOPC_PKI_Profile* pProfile, const char* applicationUri);

/**
 * \brief Set the endpoint URL used for connection to the PKI profile.
 *
 * \param pProfile A valid pointer to the PKI profile.
 * \param url The endpoint URL used for connection to set in \p pProfile.
 *
 * \warning If the URL is already defined in \p pProfile, you can not define it again.
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_ProfileSetURL(SOPC_PKI_Profile* pProfile, const char* url);

/**
 * \brief Delete a PKI profile.
 *
 * \param ppProfile The PKI profile.
 */
void SOPC_PKIProvider_DeleteProfile(SOPC_PKI_Profile** ppProfile);

/** \brief Redefines the directory store where the certificates will be stored with ::SOPC_PKIProvider_WriteToStore
 *
 * \param directoryStorePath The directory path where the certificates will be stored.
 * \param pPKI A valid pointer to the PKIProvider.
 *
 * \note The directory is created if \p directoryStorePath does not exist.
 * \warning In case of error, \p pPKI is unchanged.
 *
 * \return SOPC_STATUS_OK when successful, SOPC_STATUS_INVALID_PARAMETERS or SOPC_STATUS_NOK in case of error.
 */
SOPC_ReturnStatus SOPC_PKIProvider_SetStorePath(const char* directoryStorePath, SOPC_PKIProvider* pPKI);

/** \brief Validation function for a certificate with the PKI chain
 *
 *   It implements the validation with the certificate chain of the PKI.
 *
 * \param pPKI A valid pointer to the PKIProvider.
 * \param pToValidate A valid pointer to the Certificate to validate.
 * \param pProfile A valid pointer to the PKI profile.
 * \param[out] error Pointer to store the OpcUa error code when certificate validation failed.
 *
 * \note \p error is only set if returned status is different from SOPC_STATUS_OK.
 *       The certificate is internally stored if it is rejected.
 *
 * \warning In case of user PKI, the leaf profile part of \p pProfile is not applied to the certificate.
 *          The user leaf properties should be checked separately with ::SOPC_PKIProvider_CheckLeafCertificate .
 *
 * \return SOPC_STATUS_OK when the certificate is successfully validated, and
 *         SOPC_STATUS_INVALID_PARAMETERS or SOPC_STATUS_NOK.
 */
SOPC_ReturnStatus SOPC_PKIProvider_ValidateCertificate(SOPC_PKIProvider* pPKI,
                                                       const SOPC_CertificateList* pToValidate,
                                                       const SOPC_PKI_Profile* pProfile,
                                                       uint32_t* error);

/** \brief Write the certificate files in the updatedTrustList folder of the PKI storage.
 *         The updatedTrustList folder is created if it is missing.
 *         The format of the written files is DER.
 *         The updatedTrustList folder is organized as follows:
 *
 *         - updatedTrustList/trusted/certs
 *         - updatedTrustList/trusted/crl
 *         - updatedTrustList/issuers/certs
 *         - updatedTrustList/issuers/crl
 *
 * \param pPKI A valid pointer to the PKIProvider.
 * \param bEraseExistingFiles whether the existing files of the updatedTrustList folder shall be deleted.
 *
 * \warning If the \p pPKI is built from lists ( ::SOPC_PKIProvider_CreateFromList ) then
 *          you shall define the directory store path with ::SOPC_PKIProvider_SetStorePath .
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_WriteToStore(SOPC_PKIProvider* pPKI, const bool bEraseExistingFiles);

/** \brief Extracts certificates from the PKI object.
 *
 * \param pPKI A valid pointer to the PKIProvider.
 * \param ppTrustedCerts Write: a valid pointer to a copy of the trusted certificate list.
 *                       Append: a pointer to a pointer to a certificate list to which append the trusted certificate
 *                       list. In either cases, you should free this object.
 * \param ppTrustedCrl Write: a valid pointer to a copy of the trusted CRL list.
 *                     Append: a pointer to a pointer to a certificate list to which append the trusted CRL list.
 *                     In either cases, you should free this object.
 * \param ppIssuerCerts Write: a valid pointer to a copy of the issuer certificate list.
 *                      Append: a pointer to a pointer to a certificate list to which append the issuer certificate
 *                      list. In either cases, you should free this object.
 * \param ppIssuerCrl Write: a valid pointer to a copy of the issuer CRL list.
 *                    Append: a pointer to a pointer to a certificate list to which append the issuer CRL list.
 *                    In either cases, you should free this object.
 *
 * \note In case of error, the whole lists
 *       ( \p ppTrustedCerts , \p ppTrustedCrl , \p ppIssuerCerts and \p ppIssuerCrl ) are free and set to NULL.
 *
 * \note If the \p pPKI contains an empty list then nothing is write or append for this list.
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_WriteOrAppendToList(SOPC_PKIProvider* pPKI,
                                                       SOPC_CertificateList** ppTrustedCerts,
                                                       SOPC_CRLList** ppTrustedCrl,
                                                       SOPC_CertificateList** ppIssuerCerts,
                                                       SOPC_CRLList** ppIssuerCrl);

/** \brief Write the rejected certificates files in the rejected folder of the PKI storage.
 *         The format of the written files is DER.
 *
 * \param pPKI A valid pointer to the PKIProvider.
 *
 * \note The maximum number of certificates written in the rejected folder is \c SOPC_PKI_MAX_NB_CERT_REJECTED .
 *       This function removes the existing files.
 *
 *
 * \warning If the \p pPKI is built from lists ( ::SOPC_PKIProvider_CreateFromList ) then
 *          you shall define the directory store path with ::SOPC_PKIProvider_SetStorePath .
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_WriteRejectedCertToStore(SOPC_PKIProvider* pPKI);

/** \brief Copy the list of certificate that have been rejected.
 *
 * \param pPKI A valid pointer to the PKIProvider.
 * \param[out] ppCert A copy of the PKI rejected list (NULL if no certificate has been rejected).
 *
 * \note The maximum number of certificates returned is \c SOPC_PKI_MAX_NB_CERT_REJECTED.
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIProvider_CopyRejectedList(SOPC_PKIProvider* pPKI, SOPC_CertificateList** ppCert);

/** \brief Update the PKI with new lists of certificates and CRL.
 *
 * \param pPKI A valid pointer to the PKIProvider.
 * \param securityPolicyUri The URI describing the security policy of the secure channel.
 * \param pTrustedCerts A valid pointer to the trusted certificate list. NULL if this part shall not updated.
 * \param pTrustedCrl A valid pointer to the trusted CRL list. NULL if this part shall not updated.
 * \param pIssuerCerts A valid pointer to the issuer certificate list. NULL if this part shall not updated.
 * \param pIssuerCrl A valid pointer to the issuer CRL list. NULL if this part shall not updated.
 * \param bIncludeExistingList whether the update shall includes the existing certificates of \p pPKI plus
 *                             \p pTrustedCerts , \p pTrustedCrl , \p pIssuerCerts  and \p pIssuerCrl .
 *
 * \warning \p securityPolicyUri is not used yet and could be NULL.
 * \warning A callback shall have been defined using ::SOPC_PKIProvider_SetUpdateCb otherwise update is not authorized.
 *
 * \return SOPC_STATUS_OK when successful (SOPC_STATUS_INVALID_STATE in case update callback is not set).
 */
SOPC_ReturnStatus SOPC_PKIProvider_UpdateFromList(SOPC_PKIProvider* pPKI,
                                                  const char* securityPolicyUri,
                                                  SOPC_CertificateList* pTrustedCerts,
                                                  SOPC_CRLList* pTrustedCrl,
                                                  SOPC_CertificateList* pIssuerCerts,
                                                  SOPC_CRLList* pIssuerCrl,
                                                  const bool bIncludeExistingList);

/** \brief  Remove all the certificates matching with the given thumbprint.
 *          If the Certificate is a CA Certificate then all the CRLs for that CA are removed.
 *
 * \warning This function will fail if \p pThumbprint does not match the SHA1 hex digest size.
 * \warning A callback shall have been defined using ::SOPC_PKIProvider_SetUpdateCb otherwise update is not authorized.
 *
 * \param pPKI A valid pointer to the PKIProvider.
 * \param pThumbprint The SHA1 of the certificate formatted as an hexadecimal C string (NULL terminated)
 *                    40 bytes shall be allocated in \p pThumbprint (+ 1 byte for the NULL character)
 * \param bIsTrusted whether the certificate to remove is a trusted certificate.
 * \param[out] pIsRemoved A valid pointer indicating whether the certificate has been found and deleted.
 * \param[out] pIsIssuer A valid pointer indicating whether the deleted certificate is an issuer.
 *
 * \return SOPC_STATUS_OK when successful (SOPC_STATUS_INVALID_STATE in case update callback is not set).
 */
SOPC_ReturnStatus SOPC_PKIProvider_RemoveCertificate(SOPC_PKIProvider* pPKI,
                                                     const char* pThumbprint,
                                                     const bool bIsTrusted,
                                                     bool* pIsRemoved,
                                                     bool* pIsIssuer);

/**
 * \brief  Creates a PKI Provider without security.
 *
 * \param[out] ppPKI A valid pointer to the newly created PKIProvider. You should free such provider with
 *                   ::SOPC_PKIProvider_Free().
 * \warning  Using this PKI is considered harmful for the security of the connection.
 *           This PKI shall be used for tests or to set a new configuration from a TOFU state.
 *
 * \return SOPC_STATUS_OK when successful.
 */
SOPC_ReturnStatus SOPC_PKIPermissive_Create(SOPC_PKIProvider** ppPKI);

/**
 * \brief Defines the callback to be called when a PKI certificates update is done with
 * ::SOPC_PKIProvider_UpdateFromList or ::SOPC_PKIProvider_RemoveCertificate.
 *
 * \warning It is mandatory to define an associated behavior to allow calls to these functions.
 * \note Callback is used to re-evaluate the currently used certificates or to leave a TOFU state.
 *       (As example, ::SOPC_ToolkitClient_AsyncReEvalSecureChannels is used for the callback of the application PKI
 *        and ::SOPC_ToolkitServer_AsyncReEvalUserCertSessions is used for the callback of the user PKI)
 * \warning The callback shall not do anything blocking and shall not modify the content of its associated \p pPKI .
 *
 * \param pPKI A valid pointer to the PKIProvider for which an update callback will be defined
 * \param pUpdateCb The callback to be called when an update is done
 * \param updateParam A user defined parameter for the callback
 *
 * \return SOPC_STATUS_OK in case of success, SOPC_STATUS_INVALID_PARAMETERS in case of NULL parameter
 *         and SOPC_STATUS_INVALID_STATE in case a callback is already defined.
 */
SOPC_ReturnStatus SOPC_PKIProvider_SetUpdateCb(SOPC_PKIProvider* pPKI,
                                               SOPC_PKIProviderUpdateCb* pUpdateCb,
                                               uintptr_t updateParam);

/**
 * \brief Frees allocated PKIs.
 *
 * \param ppPKI A valid pointer to a PKI.
 */
void SOPC_PKIProvider_Free(SOPC_PKIProvider** ppPKI);

#endif /* SOPC_PKI_STACK_H_ */
