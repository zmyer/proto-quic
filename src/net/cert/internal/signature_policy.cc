// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/cert/internal/signature_policy.h"

#include "base/logging.h"
#include "net/cert/internal/cert_error_params.h"
#include "net/cert/internal/cert_errors.h"
#include "third_party/boringssl/src/include/openssl/obj.h"

namespace net {

namespace {

DEFINE_CERT_ERROR_ID(kUnacceptableCurveForEcdsa,
                     "Only P-256, P-384, P-521 are supported for ECDSA");
DEFINE_CERT_ERROR_ID(kRsaModulusTooSmall, "RSA modulus too small");

bool IsModulusSizeGreaterOrEqual(size_t modulus_length_bits,
                                 size_t min_length_bits,
                                 CertErrors* errors) {
  if (modulus_length_bits < min_length_bits) {
    errors->AddError(kRsaModulusTooSmall,
                     CreateCertErrorParams2SizeT("actual", modulus_length_bits,
                                                 "minimum", min_length_bits));
    return false;
  }
  return true;
}

}  // namespace

bool SignaturePolicy::IsAcceptableSignatureAlgorithm(
    const SignatureAlgorithm& algorithm,
    CertErrors* errors) const {
  return true;
}

bool SignaturePolicy::IsAcceptableCurveForEcdsa(int curve_nid,
                                                CertErrors* errors) const {
  switch (curve_nid) {
    case NID_X9_62_prime256v1:
    case NID_secp384r1:
    case NID_secp521r1:
      return true;
  }

  errors->AddError(kUnacceptableCurveForEcdsa);
  return false;
}

bool SignaturePolicy::IsAcceptableModulusLengthForRsa(
    size_t modulus_length_bits,
    CertErrors* errors) const {
  return IsModulusSizeGreaterOrEqual(modulus_length_bits, 2048, errors);
}

SimpleSignaturePolicy::SimpleSignaturePolicy(size_t min_rsa_modulus_length_bits)
    : min_rsa_modulus_length_bits_(min_rsa_modulus_length_bits) {}

bool SimpleSignaturePolicy::IsAcceptableModulusLengthForRsa(
    size_t modulus_length_bits,
    CertErrors* errors) const {
  return IsModulusSizeGreaterOrEqual(modulus_length_bits,
                                     min_rsa_modulus_length_bits_, errors);
}

}  // namespace net
