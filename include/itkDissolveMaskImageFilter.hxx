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

#ifndef itkDissolveMaskImageFilter_hxx
#define itkDissolveMaskImageFilter_hxx

#include "itkDissolveMaskImageFilter.h"

#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionRange.h"
#include "itkImageAlgorithm.h"

#include <algorithm>
#include <vector>
#include <queue>

namespace itk
{
template <typename TInputImage, typename TMaskImage>
DissolveMaskImageFilter<TInputImage, TMaskImage>::DissolveMaskImageFilter()
{
  this->SetNumberOfRequiredInputs(2);
  m_BackgroundValue = NumericTraits<InputPixelType>::ZeroValue();
}

template <typename TInputImage, typename TMaskImage>
void
DissolveMaskImageFilter<TInputImage, TMaskImage>::GenerateData()
{
  typename OutputImageType::Pointer     output = this->GetOutput();
  typename InputImageType::ConstPointer input = this->GetInput();
  typename MaskImageType::ConstPointer  mask = this->GetMaskImage();

  const auto region = output->GetRequestedRegion(); // TODO: check if requested region makes sense
  output->CopyInformation(input);
  output->SetRegions(input->GetRequestedRegion());
  output->Allocate(true);
  ImageAlgorithm::Copy<InputImageType, OutputImageType>(input, output, region, region);

  struct index_type
  {
    IndexType      idx;
    InputPixelType val;
  };
  std::vector<index_type> seeds;

  for (int direction = 0; direction < ImageDimension; ++direction)
  {
    ImageLinearConstIteratorWithIndex<MaskImageType> it(mask, region);
    it.SetDirection(direction);
    it.GoToBegin();
    for (; !it.IsAtEnd(); it.NextLine())
    {
      auto last_value = it.Get();
      auto last_idx = it.GetIndex();

      if (last_value) // mask at boundary
      {
        seeds.push_back({ last_idx, this->m_BackgroundValue });
      }

      for (; !it.IsAtEndOfLine(); ++it)
      {
        if (it.Get() != last_value)
        {
          if (last_value) // leaving mask
          {
            seeds.push_back({ last_idx, input->GetPixel(it.GetIndex()) });
          }
          else // entering mask
          {
            seeds.push_back({ it.GetIndex(), input->GetPixel(last_idx) });
          }
          last_value = it.Get();
        }
        last_idx = it.GetIndex();
      }

      if (last_value) // mask at boundary
      {
        seeds.push_back({ last_idx, this->m_BackgroundValue });
      }
    }
  }

  // for progress reporting
  size_t                                     mask_size = 0;
  itk::ImageRegionRange<const MaskImageType> image_range(*mask, region);
  for (auto && pixel : image_range)
  {
    mask_size += (pixel != 0 ? 1 : 0);
  }

  auto geodesic_distance = itk::Image<float, ImageDimension>::New();
  geodesic_distance->SetRegions(region);
  geodesic_distance->Allocate();
  geodesic_distance->FillBuffer(std::numeric_limits<float>::max());

  using node = std::pair<float, index_type>;
  const auto cmp = [](const node & l, const node & r) { return l.first > r.first; };
  std::priority_queue<node, std::vector<node>, decltype(cmp)> queue(cmp);
  for (const auto & s : seeds)
  {
    geodesic_distance->SetPixel(s.idx, 0.f);
    queue.push(std::make_pair(0.f, s));
  }

  auto         neighbors = this->GetNeighborOffsets();
  auto         deltas = this->GetNeighborDeltas(neighbors, input->GetSpacing());
  const size_t num_neighbors = neighbors.size();

  ProgressReporter progress(this, 0, mask_size, 100);
  while (!queue.empty())
  {
    auto   node = queue.top().second; // node
    double dist = queue.top().first;  // node cost
    queue.pop();

    // if we already have better cost, skip the currently popped node
    if (geodesic_distance->GetPixel(node.idx) < dist)
      continue;

    // set pixel value to closest value outside mask
    output->SetPixel(node.idx, node.val);
    progress.CompletedPixel();

    // update neighboring values
    for (size_t k = 0; k < num_neighbors; ++k)
    {
      const float     n_dist = dist + deltas[k];
      const IndexType n_id = node.idx + neighbors[k];

      // check if neighbor is in region
      if (!region.IsInside(n_id))
        continue;

      if (mask->GetPixel(n_id) && geodesic_distance->GetPixel(n_id) > n_dist)
      {
        // add to queue
        geodesic_distance->SetPixel(n_id, n_dist);
        queue.push(std::make_pair(n_dist, index_type{ n_id, node.val }));
      }
    }
  }
}

template <typename TInputImage, typename TMaskImage>
std::vector<float>
DissolveMaskImageFilter<TInputImage, TMaskImage>::GetNeighborDeltas(const std::vector<OffsetType> & offsets,
                                                                    const SpacingType &             spacing)
{
  std::vector<float> deltas;
  for (const auto & offset : offsets)
  {
    float val = 0.f;
    for (unsigned i = 0; i < ImageDimension; ++i)
    {
      float d = static_cast<float>(offset[i] * spacing[i]);
      val += d * d;
    }
    deltas.push_back(std::sqrt(val));
  }
  return deltas;
}

/**
 * Standard "PrintSelf" method
 */
template <typename TInputImage, typename TMaskImage>
void
DissolveMaskImageFilter<TInputImage, TMaskImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent
     << "Background value : " << static_cast<typename NumericTraits<InputPixelType>::PrintType>(m_BackgroundValue)
     << std::endl;
}
} // end namespace itk

#endif
