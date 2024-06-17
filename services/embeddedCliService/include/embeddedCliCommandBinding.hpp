/// @brief  The Embedded-CLI Service, CommandBinding structure
/// @ingroup
/// @cond
///***************************************************************************
///
/// License is noted in the service's LICENSE.txt file (MIT)
///
/// Contact Information:
///   Matthew Eshleman
///   https://covemountainsoftware.com
///   info@covemountainsoftware.com
///***************************************************************************
/// @endcond

#ifndef CMS_EMBEDDED_CLI_COMMAND_BINDING_HPP
#define CMS_EMBEDDED_CLI_COMMAND_BINDING_HPP

#include <cstdbool>

// forward declare the third-party EmbeddedCli structs
struct EmbeddedCli;

namespace cms {
namespace EmbeddedCLI {   // note, all caps CLI needed to avoid conflicts
/**
 * This is basically a copy of the same struct from embedded-cli.
 * Going a bit out of my way to avoid exposing embedded-cli details
 * outside this service.
 */
struct CommandBinding {
    /**
     * Name of command to bind. Should not be NULL.
     */
    const char* name;

    /**
     * Help string that will be displayed when "help <cmd>" is executed.
     * Can have multiple lines separated with "\r\n"
     * Can be NULL if no help is provided.
     */
    const char* help;

    /**
     * Flag to perform tokenization before calling binding function.
     */
    bool tokenizeArgs;

    /**
     * Pointer to any specific app context that is required for this binding.
     * It will be provided in binding callback.
     */
    void* context;

    /**
     * Binding function for when command is received.
     * @param cli - pointer to cli that is calling this binding
     * @param args - string of args (if tokenizeArgs is false) or tokens otherwise
     * @param context
     */
    void (*binding)(EmbeddedCli* cli, char* args, void* context);
};

} //namespace EmbeddedCLI
} //namespace cms

#endif   // CMS_EMBEDDED_CLI_COMMAND_BINDING_HPP
