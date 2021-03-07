import re
from typing import List


def numeric_to_alpha(num_list: List[int]) -> str:
    """Covert numeric operator to alphabetical one.

    Args:
        num_list (List[int]): operator in numberic form

    Raises:
        TypeError: number in num_list should be in [0, 52)

    Returns:
        str: alphabetical form
    """
    # make sure only [A-Za-z] in the `num_list`
    for num in num_list:
        if num <= 0 and num > 52:
            raise TypeError(f"num_list is invalid: {repr(num_list)}")
    # return ans
    ret = ""
    # convert
    for num in num_list:
        if num % 2 == 1:
            # [A-Z]
            offset = int((num - 1) / 2)
            ret += chr(ord("A") + offset)
        else:
            # [a-z]
            offset = int((num - 1) / 2)
            ret += chr(ord("a") + offset)
    return ret


def alpha_to_numeric(alpha: str) -> List[int]:
    """Convert alphabetical operator to numberic one.

    Args:
        alpha_list (str): alphabetical operator

    Raises:
        TypeError: letter in `alpha` must be alphabetical

    Returns:
        List[int]: numeric form
    """
    # make sure only letters
    if re.match("^[A-Za-z]+$", alpha) is None:
        raise TypeError(f"alpha_list is invalid: {repr(alpha)}")
    # return ans
    ret: List[int] = []
    # convert
    for letter in alpha:
        c = ord(letter)
        if c >= ord("A") and c <= ord("Z"):
            ret.append(c - ord("A") + 1)
        else:
            ret.append((c - ord("a") + 1) * 2)
    return ret
