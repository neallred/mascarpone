import re
import socket

domain_regex = r"(([\da-zA-Z])([_\w-]{,62})\.){,127}(([\da-zA-Z])[_\w-]{,61})?([\da-zA-Z]\.((xn\-\-[a-zA-Z\d]+)|([a-zA-Z\d]{2,})))"

domain_regex = "{0}$".format(domain_regex)
valid_domain_name_regex = re.compile(domain_regex, re.IGNORECASE)


def validate_domain(domain):
    domain_name = domain.lower().strip()
    if re.match(valid_domain_name_regex, domain_name):
        return True
    else:
        return False


def _is_valid_ipv4_address(address):
    try:
        socket.inet_pton(socket.AF_INET, address)
    except AttributeError:  # no inet_pton here, sorry
        try:
            socket.inet_aton(address)
        except socket.error:
            return False
        return address.count(".") == 3
    except socket.error:  # not a valid address
        return False

    return True


def _is_valid_ipv6_address(address):
    try:
        socket.inet_pton(socket.AF_INET6, address)
    except socket.error:  # not a valid address
        return False
    return True


def validate_ip_address(ip_address):
    return _is_valid_ipv4_address(ip_address) or _is_valid_ipv6_address(ip_address)


def validate_delete(body):
    domain = None
    errors = []
    try:
        domain = body["domain"]
    except:
        errors.append("Missing domain info")

    if (domain or domain == "") and not validate_domain(domain):
        errors.append(f'"{domain}" is not a valid domain')

    return errors

def validate_upsert(body):
    domain = None
    redirect = None
    errors = []
    try:
        domain = body["domain"]
    except:
        errors.append("Missing domain info")
    try:
        redirect = body["redirect"]
    except:
        errors.append("Missing redirect info")

    if (domain or domain == "") and not validate_domain(domain):
        errors.append(f'"{domain}" is not a valid domain')
    if (redirect or redirect == "") and not validate_ip_address(redirect):
        errors.append(f'"{redirect}" is not a valid ip address to redirect to.')

    return errors

def validate_update(body):
    domain = None
    new_domain = None
    redirect = None
    errors = []
    try:
        domain = body["domain"]
    except:
        errors.append("Missing domain info")
    try:
        new_domain = body["newDomain"]
    except:
        errors.append("Missing new domain info")
    try:
        redirect = body["redirect"]
    except:
        errors.append("Missing redirect info")

    if (domain or domain == "") and not validate_domain(domain):
        errors.append(f'"{domain}" is not a valid domain')
    if (new_domain or new_domain == "") and not validate_domain(new_domain):
        errors.append(f'"{new_domain}" is not a valid domain')
    if (redirect or redirect == "") and not validate_ip_address(redirect):
        errors.append(f'"{redirect}" is not a valid ip address to redirect to.')

    return errors
