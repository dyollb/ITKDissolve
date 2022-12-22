/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkDissolveMaskImageFilter.h"

#include "itkCommand.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"

namespace
{
class ShowProgress : public itk::Command
{
public:
  itkNewMacro(ShowProgress);

  void
  Execute(itk::Object * caller, const itk::EventObject & event) override
  {
    Execute((const itk::Object *)caller, event);
  }

  void
  Execute(const itk::Object * caller, const itk::EventObject & event) override
  {
    if (!itk::ProgressEvent().CheckEvent(&event))
    {
      return;
    }
    const auto * processObject = dynamic_cast<const itk::ProcessObject *>(caller);
    if (!processObject)
    {
      return;
    }
    std::cout << " " << processObject->GetProgress();
  }
};
} // namespace

int
itkDissolveMaskImageFilterTest(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv);
    std::cerr << " outputImage";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }
  const char * outputImageFileName = argv[1];

  constexpr unsigned int Dimension = 3;
  using PixelType = unsigned char;
  using ImageType = itk::Image<PixelType, Dimension>;

  using FilterType = itk::DissolveMaskImageFilter<ImageType, ImageType>;
  auto filter = FilterType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(filter, DissolveMaskImageFilter, ImageToImageFilter);

  // Create input image to avoid test dependencies.
  ImageType::SizeType size = { 128, 128, 128 };
  ImageType::Pointer  image = ImageType::New();
  image->SetRegions(size);
  image->Allocate();
  image->FillBuffer(0);

  ImageType::RegionType region;
  region.SetIndex({ 0, 0, 0 });
  region.SetSize({ 128, 128, 64 });

  for (auto && pixel : itk::ImageRegionRange<ImageType>(*image, region))
  {
    pixel = 1;
  }

  // Create mask
  ImageType::Pointer mask = ImageType::New();
  mask->SetRegions(size);
  mask->Allocate();
  mask->FillBuffer(0);

  ImageType::RegionType mask_region;
  mask_region.SetIndex({ 30, 20, 50 });
  mask_region.SetSize({ 10, 20, 30 });
  for (auto && pixel : itk::ImageRegionRange<ImageType>(*mask, mask_region))
  {
    pixel = 1;
  }

  ShowProgress::Pointer showProgress = ShowProgress::New();
  filter->AddObserver(itk::ProgressEvent(), showProgress);
  filter->SetInput(image);
  filter->SetMaskImage(mask);

  using WriterType = itk::ImageFileWriter<ImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputImageFileName);
  writer->SetInput(filter->GetOutput());
  writer->SetUseCompression(true);

  ITK_TRY_EXPECT_NO_EXCEPTION(writer->Update());

  std::cout << "Test finished." << std::endl;
  return EXIT_SUCCESS;
}
