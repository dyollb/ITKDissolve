/// Author Bryn Lloyd
///
/// Copyright (c) 2022 IT'IS Foundation


#ifndef itkDissolveMaskImageFilter_h
#define itkDissolveMaskImageFilter_h

#include "itkImageToImageFilter.h"

namespace itk
{
/** \class DissolveMaskImageFilter
 * \brief Discard the label values inside a mask
 *
 * \note The pixels inside the mask are dissolved by assigning values
 * from neighboring region. The filter processes in an ordered fashion
 * from the mask boundary towards the inside.
 *
 */
template <typename TInputImage, typename TMaskImage, typename TOutputImage = TInputImage>
class ITK_TEMPLATE_EXPORT DissolveMaskImageFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Extract dimension from input and output image. */
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

  static const unsigned int ImageDimension = InputImageDimension;

  /** Convenient type aliases for simplifying declarations. */
  using InputImageType = TInputImage;
  using MaskImageType = TMaskImage;
  using OutputImageType = TOutputImage;

  /** Standard class type aliases. */
  using Self = DissolveMaskImageFilter;
  using Superclass = ImageToImageFilter<InputImageType, OutputImageType>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DissolveMaskImageFilter, ImageToImageFilter);

  /** Image type support. */
  using InputPixelType = typename InputImageType::PixelType;
  using MaskPixelType = typename MaskImageType::PixelType;
  using OutputPixelType = typename OutputImageType::PixelType;

  using IndexType = typename InputImageType::IndexType;
  using OffsetType = typename InputImageType::OffsetType;
  using SpacingType = typename InputImageType::SpacingType;

  /** Set/Get the mask image. Pixels set in the mask image will retain
   *  the original value of the input image while pixels not set in
   *  the mask will be set to the "OutsideValue".
   */
  void
  SetMaskImage(const MaskImageType * maskImage)
  {
    // Process object is not const-correct so the const casting is required.
    this->SetNthInput(1, const_cast<MaskImageType *>(maskImage));
  }
  const MaskImageType *
  GetMaskImage()
  {
    return static_cast<const MaskImageType *>(this->ProcessObject::GetInput(1));
  }

  /** Set the value associated with the Background . */
  itkSetMacro(BackgroundValue, InputPixelType);

  itkGetConstReferenceMacro(BackgroundValue, InputPixelType);

  /** Returns the number of pixels that changed when the filter was executed. */
  itkGetConstReferenceMacro(NumberOfPixelsChanged, SizeValueType);

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro(InputEqualityComparableCheck, (Concept::EqualityComparable<InputPixelType>));
  itkConceptMacro(IntConvertibleToInputCheck, (Concept::Convertible<int, InputPixelType>));
  itkConceptMacro(InputConvertibleToOutputCheck, (Concept::Convertible<InputPixelType, OutputPixelType>));
  itkConceptMacro(SameDimensionCheck, (Concept::SameDimension<InputImageDimension, OutputImageDimension>));
  itkConceptMacro(InputOStreamWritableCheck, (Concept::OStreamWritable<InputPixelType>));
  // End concept checking
#endif

protected:
  DissolveMaskImageFilter();
  ~DissolveMaskImageFilter() override {}
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  template <unsigned int VDim>
  std::vector<Offset<VDim>>
  GetNeighborOffsets();

  template <>
  std::vector<Offset<2>>
  GetNeighborOffsets<2>()
  {
    return { Offset<2>{ -1, 0 }, Offset<2>{ 1, 0 }, Offset<2>{ 0, -1 }, Offset<2>{ 0, 1 } };
  }

  template <>
  std::vector<Offset<3>>
  GetNeighborOffsets<3>()
  {
    return { Offset<3>{ -1, 0, 0 }, Offset<3>{ 1, 0, 0 },  Offset<3>{ 0, -1, 0 },
             Offset<3>{ 0, 1, 0 },  Offset<3>{ 0, 0, -1 }, Offset<3>{ 0, 0, 1 } };
  }

  std::vector<float>
  GetNeighborDeltas(const std::vector<OffsetType> & offsets, const SpacingType & spacing);

  void
  GenerateData() override;

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(DissolveMaskImageFilter);

  InputPixelType m_BackgroundValue;

  SizeValueType m_NumberOfPixelsChanged = 0;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkDissolveMaskImageFilter.hxx"
#endif

#endif