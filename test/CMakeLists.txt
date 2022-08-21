itk_module_test()

set(DissolveTests
  itkDissolveMaskImageFilterTest.cxx
  )

CreateTestDriver(Dissolve "${Dissolve-Test_LIBRARIES}" "${DissolveTests}")

set(TEST_DATA_ROOT "${Dissolve_SOURCE_DIR}/Data")

itk_add_test(NAME itkDissolveMaskImageFilterTest
  COMMAND DissolveTestDriver
    --compare
    ${TEST_DATA_ROOT}/Baseline/itkDissolveMaskImageFilterTestOutput.mha
    ${ITK_TEST_OUTPUT_DIR}/itkDissolveMaskImageFilterTestOutput.mha
  itkDissolveMaskImageFilterTest
    ${ITK_TEST_OUTPUT_DIR}/itkDissolveMaskImageFilterTestOutput.mha
  )
