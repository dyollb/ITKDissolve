ITKDissolve
=================================

.. image:: https://github.com/dyollb/ITKDissolve/workflows/Build,%20test,%20package/badge.svg
    :alt:    Build Status

.. image:: https://img.shields.io/pypi/v/itk-dissolve.svg
    :target: https://pypi.python.org/pypi/itk-dissolve
    :alt: PyPI Version

.. image:: https://img.shields.io/github/license/dyollb/ITKDissolve?color=blue
    :target: https://github.com/dyollb/ITKDissolve/blob/main/LICENSE
    :alt: License

Overview
--------

This is a module for the Insight Toolkit (ITK) that provides functionality to discard pixels within a masked region. Unlike masking, the pixels in the masked region are replaced (dissolved) by their nearest pixels outside the mas. This can be useful to clean-up label fields, e.g. to remove small islands/holes or remove entire labels, and replace them by the adjacent labels.

The module includes a filter called DissolveMaskImageFilter.

.. code-block:: python

    import itk
    labels = itk.imread('path/to/labels.mha').astype(itk.US)
    mask = itk.imread('path/to/mask.mha').astype(itk.UC)

    ImageType = type(labels)
    MaskType = type(mask)

    dissolve = itk.DissolveMaskImageFilter[ImageType, MaskType].New()
    dissolve.SetInput(labels)
    dissolve.SetMaskImage(mask)
    dissolve.Update()
    modified_labels = dissolve.GetOutput()

    itk.imwrite(modified_labels, 'modified_labels2.mha')


Or using the pythonic API:

.. code-block:: python

    import itk
    labels = itk.imread('path/to/labels.mha').astype(itk.US)
    mask = itk.imread('path/to/mask.mha').astype(itk.UC)
    modified_labels = itk.dissolve_mask_image_filter(labels, mask_image=mask)
    itk.imwrite(modified_labels, 'path/to/modified_labels.mha')


Installation
------------

To install the binary Python packages::

  python -m pip install itk-dissolve