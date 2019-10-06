import pytest

import mascarpone.validators as v


@pytest.mark.parametrize(
    "ip_address",
    [
        "192.168.0.135",
        "1.1.1.1",
        "8.8.8.8",
        "216.49.176.43",
        "255.255.255.255",
        "2620:0:ccc::2",
        "2001:4860:4860::8888",
    ],
)
def test_valid_ipv6_is_marked_valid(ip_address):
    assert v.validate_ip_address(ip_address)


@pytest.mark.parametrize("ip_address", ["", "1.1.256.1", "8.8.8.", "8.8.8"])
def test_invalid_ip_addresses_marked_invalid(ip_address):
    assert not v.validate_ip_address(ip_address)


@pytest.mark.parametrize(
    "domain",
    [
        "evil.org",
        "aa.aa",
        "ads.linkedin.com",
        "ad.gt",
        "analytics.yahoo.com",
        "clicktale.net",
        "dwin2.com",
        "hs-analytics.net",
        "this_is.fine",
    ],
)
def test_valid_domains(domain):
    assert v.validate_domain(domain)


@pytest.mark.parametrize(
    "domain",
    [
        "a.a",
        "",
        "this_is_not_a_domain",
        "thisisnotadomaineither",
        "__.__",
        "this_is_.notfine",
    ],
)
def test_invalid_domains(domain):
    assert not v.validate_domain(domain)


def test_valid_upsert():
    assert not v.validate_upsert({"domain": "evil.org", "redirect": "127.0.0.1"})


def test_validate_upsert_does_not_double_errors():
    assert len(v.validate_upsert({})) == 2
    assert len(v.validate_upsert({"domain": "", "insert": ""})) == 2
